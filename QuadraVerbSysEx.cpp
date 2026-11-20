#include "QuadraVerbSysEx.h"

// ------------------------------------------------------------------ packing
void QuadraVerbSysEx::pack16 (juce::uint16 v, juce::uint8* o) noexcept
{
    o[0] = (juce::uint8) ((v >> 14) & 0x03);
    o[1] = (juce::uint8) ((v >> 7) & 0x7F);
    o[2] = (juce::uint8) (v & 0x7F);
}
juce::uint16 QuadraVerbSysEx::unpack16 (const juce::uint8* i) noexcept
{
    return (juce::uint16) (((i[0] & 0x03) << 14) | ((i[1] & 0x7F) << 7) | (i[2] & 0x7F));
}

juce::MidiMessage QuadraVerbSysEx::makeMessage (juce::uint8 func, juce::uint8 page, juce::uint16 value, juce::uint8 deviceId)
{
    juce::uint8 body[10] = { kManufacturer[0], kManufacturer[1], kManufacturer[2], kProductId, (juce::uint8) (deviceId & 0x7F),
                             (juce::uint8) (func & 0x7F), (juce::uint8) (page & 0x7F), 0, 0, 0 };
    pack16 (value, body + 7);
    return juce::MidiMessage::createSysExMessage (body, (int) sizeof (body));   // JUCE adds the F0 and F7
}

bool QuadraVerbSysEx::parseMessage (const juce::MidiMessage& m, Packet& p)
{
    if (! m.isSysEx() || m.getSysExDataSize() != 10) return false;
    const auto* d = m.getSysExData();
    for (int i = 0; i < 10; ++i) if (d[i] > 0x7F) return false;
    if (d[0] != kManufacturer[0] || d[1] != kManufacturer[1] || d[2] != kManufacturer[2] || d[3] != kProductId) return false;
    p.deviceId = d[4]; p.func = d[5]; p.page = d[6]; p.value = unpack16 (d + 7);
    return true;
}

juce::Array<juce::MidiMessage> QuadraVerbSysEx::splitSyx (const void* data, size_t size)
{
    juce::Array<juce::MidiMessage> out; const auto* b = static_cast<const juce::uint8*> (data);
    std::vector<juce::uint8> cur; bool inside = false;
    for (size_t i = 0; i < size; ++i)
    {
        const juce::uint8 c = b[i];
        if (c == 0xF0) { cur.assign (1, c); inside = true; }
        else if (! inside) continue;                                   // bytes between messages are ignored
        else if (c == 0xF7) { cur.push_back (c); out.add (juce::MidiMessage (cur.data(), (int) cur.size())); inside = false; }
        else if (c >= 0x80) inside = false;                            // another status byte: the message was cut short, drop it
        else cur.push_back (c);
    }
    return out;
}

// ------------------------------------------------------------------ rings (single producer / single consumer, no allocation)
bool QuadraVerbSysEx::Ring::push (const juce::MidiMessage& m)
{
    const int n = m.getRawDataSize(); if (n <= 0 || n > kSlotBytes) return false;
    int s1, n1, s2, n2; fifo.prepareToWrite (1, s1, n1, s2, n2);
    if (n1 < 1) return false;
    Slot& s = slots[(size_t) s1]; s.size = n; std::memcpy (s.data.data(), m.getRawData(), (size_t) n);
    fifo.finishedWrite (1); return true;
}
template <typename F> void QuadraVerbSysEx::Ring::popAll (F&& f)
{
    for (;;)
    {
        int s1, n1, s2, n2; fifo.prepareToRead (1, s1, n1, s2, n2);
        if (n1 < 1) return;
        const Slot& s = slots[(size_t) s1]; f (s.data.data(), s.size);
        fifo.finishedRead (1);
    }
}

// ------------------------------------------------------------------ life cycle and the 15 ms throttle
QuadraVerbSysEx::QuadraVerbSysEx() { startTimer (kIntervalMs); }
QuadraVerbSysEx::~QuadraVerbSysEx() { stopTimer(); if (hwIn) hwIn->stop(); }

void QuadraVerbSysEx::enqueue (const juce::MidiMessage& m)
{
    const juce::ScopedLock sl (lock);
    if (queue.size() >= 4096) { ++dropped; return; }
    queue.push_back (m);
}
void QuadraVerbSysEx::enqueueParam (juce::uint8 func, juce::uint8 page, juce::uint16 value) { enqueue (makeMessage (func, page, value)); }
int  QuadraVerbSysEx::queueSize() const { const juce::ScopedLock sl (lock); return (int) queue.size(); }
void QuadraVerbSysEx::clearQueue() { const juce::ScopedLock sl (lock); queue.clear(); holdTicks = 0; }

void QuadraVerbSysEx::deliver (const juce::MidiMessage& m)
{
    if (testSink) { testSink (m); return; }
    if (useHardware && hwOut) hwOut->sendMessageNow (m);
    if (usePlugin && ! outRing.push (m)) ++dropped;
}

void QuadraVerbSysEx::timerCallback()
{
    // incoming first (cheap): host SysEx handed over by the audio thread, and anything the hardware callback queued
    inRing.popAll ([this] (const juce::uint8* d, int n) { handleIncoming (juce::MidiMessage (d, n)); });
    for (;;)
    {
        juce::MidiMessage m;
        { const juce::ScopedLock sl (lock); if (hwIncoming.empty()) break; m = hwIncoming.front(); hwIncoming.pop_front(); }
        handleIncoming (m);
    }
    // outgoing: one message per tick, then wait out the transmission time of what was just sent
    if (holdTicks > 0) { --holdTicks; return; }
    juce::MidiMessage m;
    { const juce::ScopedLock sl (lock); if (queue.empty()) return; m = queue.front(); queue.pop_front(); }
    deliver (m); ++sent;
    if (wireAware) holdTicks = juce::jmax (0, (int) std::ceil ((double) m.getRawDataSize() * 0.32 / kIntervalMs) - 1);   // 10 bits per byte at 31.25 kbaud
}

// ------------------------------------------------------------------ devices
bool QuadraVerbSysEx::openOutput (const juce::String& id)
{
    hwOut.reset(); outId = {};
    if (id.isEmpty()) return true;
    hwOut = juce::MidiOutput::openDevice (id);
    if (hwOut) outId = id;
    return hwOut != nullptr;
}
bool QuadraVerbSysEx::openInput (const juce::String& id)
{
    if (hwIn) hwIn->stop();
    hwIn.reset(); inId = {};
    if (id.isEmpty()) return true;
    hwIn = juce::MidiInput::openDevice (id, this);
    if (hwIn) { inId = id; hwIn->start(); }
    return hwIn != nullptr;
}

void QuadraVerbSysEx::drainOutputTo (juce::MidiBuffer& midi)   // audio thread
{
    outRing.popAll ([&] (const juce::uint8* d, int n) { midi.addEvent (d, n, 0); });
}
void QuadraVerbSysEx::pushHostSysEx (const juce::MidiMessage& m) { if (m.isSysEx() && ! inRing.push (m)) ++dropped; }   // audio thread
void QuadraVerbSysEx::receive (const juce::MidiMessage& m)       // MIDI thread
{
    if (! m.isSysEx()) return;
    const juce::ScopedLock sl (lock); if (hwIncoming.size() < 256) hwIncoming.push_back (m);
}

void QuadraVerbSysEx::handleIncoming (const juce::MidiMessage& m)   // message thread
{
    if (! m.isSysEx()) return;
    ++received;
    juce::String t; const int n = juce::jmin (m.getRawDataSize(), 24);
    for (int i = 0; i < n; ++i) t << juce::String::toHexString (m.getRawData()[i]).paddedLeft ('0', 2) << ' ';
    if (m.getRawDataSize() > n) t << "...";
    { const juce::ScopedLock sl (lock); lastRxText = t.trim();
      if (capturing) dump.emplace_back (m.getRawData(), m.getRawData() + m.getRawDataSize()); }
    Packet p; if (onPacket && parseMessage (m, p)) onPacket (p);
}
juce::String QuadraVerbSysEx::lastRx() const { const juce::ScopedLock sl (lock); return lastRxText; }

// ------------------------------------------------------------------ dumps
int  QuadraVerbSysEx::dumpMessageCount() const { const juce::ScopedLock sl (lock); return (int) dump.size(); }
int  QuadraVerbSysEx::dumpByteCount() const { const juce::ScopedLock sl (lock); int n = 0; for (auto& m : dump) n += (int) m.size(); return n; }
void QuadraVerbSysEx::clearDump() { const juce::ScopedLock sl (lock); dump.clear(); }
void QuadraVerbSysEx::setDump (const juce::Array<juce::MidiMessage>& msgs)
{
    const juce::ScopedLock sl (lock); dump.clear();
    for (auto& m : msgs) if (m.isSysEx()) dump.emplace_back (m.getRawData(), m.getRawData() + m.getRawDataSize());
}
juce::Array<juce::MidiMessage> QuadraVerbSysEx::getDump() const
{
    const juce::ScopedLock sl (lock); juce::Array<juce::MidiMessage> out;
    for (auto& m : dump) out.add (juce::MidiMessage (m.data(), (int) m.size()));
    return out;
}
int QuadraVerbSysEx::sendDump() { auto d = getDump(); for (auto& m : d) enqueue (m); return d.size(); }

bool QuadraVerbSysEx::exportSyx (const juce::File& f) const
{
    juce::MemoryBlock mb; { const juce::ScopedLock sl (lock); for (auto& m : dump) mb.append (m.data(), m.size()); }
    if (mb.getSize() == 0) return false;
    return f.replaceWithData (mb.getData(), mb.getSize());
}
int QuadraVerbSysEx::importSyx (const juce::File& f)
{
    juce::MemoryBlock mb; if (! f.existsAsFile() || ! f.loadFileAsData (mb)) return -1;
    auto msgs = splitSyx (mb.getData(), mb.getSize());
    if (msgs.isEmpty()) return -1;
    setDump (msgs); return msgs.size();
}

void QuadraVerbSysEx::saveState (juce::ValueTree& parent) const
{
    juce::MemoryBlock mb; { const juce::ScopedLock sl (lock); for (auto& m : dump) mb.append (m.data(), m.size()); }
    juce::ValueTree t ("SYSEX");
    t.setProperty ("dump", mb.getSize() ? juce::Base64::toBase64 (mb.getData(), mb.getSize()) : juce::String(), nullptr);   // the raw dump
    t.setProperty ("outId", outId, nullptr); t.setProperty ("inId", inId, nullptr);
    t.setProperty ("hw", useHardware, nullptr); t.setProperty ("plugin", usePlugin, nullptr); t.setProperty ("mirror", mirror, nullptr);
    auto old = parent.getChildWithName ("SYSEX"); if (old.isValid()) parent.removeChild (old, nullptr);
    parent.appendChild (t, nullptr);
}
void QuadraVerbSysEx::loadState (const juce::ValueTree& parent)
{
    auto t = parent.getChildWithName ("SYSEX"); if (! t.isValid()) return;
    juce::MemoryOutputStream out; const auto b64 = t["dump"].toString();
    if (b64.isNotEmpty() && juce::Base64::convertFromBase64 (out, b64)) setDump (splitSyx (out.getData(), out.getDataSize()));
    else clearDump();
    useHardware = (bool) t.getProperty ("hw", true); usePlugin = (bool) t.getProperty ("plugin", true); mirror = (bool) t.getProperty ("mirror", false);
    const auto o = t["outId"].toString(), i = t["inId"].toString();   // devices that have gone away are simply skipped
    if (o != outId) openOutput (o);
    if (i != inId) openInput (i);
}
