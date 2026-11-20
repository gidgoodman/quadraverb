#pragma once
#include <juce_audio_utils/juce_audio_utils.h>
#include <array>
#include <deque>
#include <functional>
#include <vector>

/**
    SysEx transport for the Alesis QuadraVerb (Plus).

    Wire format (as specified for this project):

        F0 00 00 0E 02 01 <func> <page> <v1> <v2> <v3> F7
           |_____| |  |
           Alesis  |  device id
                   product id

    A 16-bit value travels in three 7-bit bytes:  v1 = bits 15..14, v2 = bits 13..7, v3 = bits 6..0.

    NOTE: the Alesis manufacturer id (00 00 0E) is real; the product id, the func codes and the meaning of <page> are NOT
    documented in the QuadraVerb reference manual we have (it only describes the MIDI Data Dump feature), so kFuncParamSet
    and the page map in SysExMap.h are placeholders for you to confirm against the real unit / Alesis MIDI implementation.

    Outgoing messages are never sent in a burst: a background timer releases one queued message every 15 ms (longer after
    large dumps, so the hardware's MIDI buffer isn't overrun). Messages can go to a hardware MIDI port opened directly by the
    plugin (hosts such as Ableton Live don't deliver SysEx from audio plugins) and/or out of the plugin's own MIDI output.
*/
class QuadraVerbSysEx : private juce::Timer, private juce::MidiInputCallback
{
public:
    static constexpr juce::uint8 kManufacturer[3] = { 0x00, 0x00, 0x0E };   // Alesis
    static constexpr juce::uint8 kProductId = 0x02, kDefaultDeviceId = 0x01;
    enum Func : juce::uint8 { kFuncParamSet = 0x01 };                        // ASSUMED value: confirm against the unit
    static constexpr int kPacketBytes = 12;                                  // F0 ... F7
    static constexpr int kIntervalMs = 15;                                   // gap between outgoing messages
    static constexpr int kSlotBytes = 2048;                                  // largest message passed through the plugin MIDI port

    struct Packet { juce::uint8 deviceId = kDefaultDeviceId, func = 0, page = 0; juce::uint16 value = 0; };

    QuadraVerbSysEx();
    ~QuadraVerbSysEx() override;

    // ---- (1) 16-bit <-> 3 x 7-bit packing, message building and parsing
    static void pack16 (juce::uint16 v, juce::uint8* out3) noexcept;
    static juce::uint16 unpack16 (const juce::uint8* in3) noexcept;
    static juce::MidiMessage makeMessage (juce::uint8 func, juce::uint8 page, juce::uint16 value, juce::uint8 deviceId = kDefaultDeviceId);
    static bool parseMessage (const juce::MidiMessage& m, Packet& out);
    static juce::Array<juce::MidiMessage> splitSyx (const void* data, size_t size);   // every complete F0..F7 message in a byte stream

    // ---- (2) throttled output
    void enqueue (const juce::MidiMessage& m);                       // any thread
    void enqueueParam (juce::uint8 func, juce::uint8 page, juce::uint16 value);
    int  queueSize() const;
    void clearQueue();
    void setWireAwareThrottle (bool on) { wireAware = on; }          // add the 31.25 kbaud transmission time of big messages (default on)

    // ---- (3) destinations: direct hardware port and/or the plugin's MIDI output
    static juce::Array<juce::MidiDeviceInfo> getOutputDevices() { return juce::MidiOutput::getAvailableDevices(); }
    static juce::Array<juce::MidiDeviceInfo> getInputDevices()  { return juce::MidiInput::getAvailableDevices(); }
    bool openOutput (const juce::String& identifier);                // "" closes
    bool openInput  (const juce::String& identifier);                // "" closes
    juce::String outputId() const { return outId; }
    juce::String inputId()  const { return inId; }
    bool outputIsOpen() const { return hwOut != nullptr; }
    bool inputIsOpen()  const { return hwIn != nullptr; }
    void setUseHardwareOutput (bool b) { useHardware = b; }
    void setUsePluginOutput (bool b)   { usePlugin = b; }
    bool usesHardwareOutput() const { return useHardware; }
    bool usesPluginOutput()   const { return usePlugin; }
    void setMirrorEnabled (bool b) { mirror = b; }                    // send panel edits as parameter packets (needs the page map)
    bool mirrorEnabled() const { return mirror; }
    // audio thread: append queued messages to the host buffer / hand incoming host SysEx to the message thread
    void drainOutputTo (juce::MidiBuffer& midi);
    void pushHostSysEx (const juce::MidiMessage& m);
    // message thread: incoming from a hardware port (called by the MIDI callback) or injected by tests
    void receive (const juce::MidiMessage& m);

    // ---- (4) dumps: capture, import/export .syx, keep with the plugin state
    void setCapture (bool on) { capturing = on; }
    bool isCapturing() const { return capturing; }
    int  dumpMessageCount() const;
    int  dumpByteCount() const;
    void clearDump();
    void setDump (const juce::Array<juce::MidiMessage>& msgs);
    juce::Array<juce::MidiMessage> getDump() const;
    int  sendDump();                                                  // queues every captured message; returns how many
    bool exportSyx (const juce::File& f) const;                       // standard .syx: the messages back to back
    int  importSyx (const juce::File& f);                             // returns the number of messages loaded, or -1
    void saveState (juce::ValueTree& parent) const;                   // adds/replaces a "SYSEX" child (raw dump as base64 + settings)
    void loadState (const juce::ValueTree& parent);

    // ---- status and hooks
    std::function<void (const Packet&)> onPacket;                     // a parameter packet arrived (message thread)
    juce::String lastRx() const;
    int sentCount() const { return sent; }
    int receivedCount() const { return received; }
    int droppedCount() const { return dropped; }
    std::function<void (const juce::MidiMessage&)> testSink;          // when set, replaces the real destinations (unit tests)

private:
    struct Slot { int size = 0; std::array<juce::uint8, kSlotBytes> data {}; };
    struct Ring
    {
        juce::AbstractFifo fifo { 32 }; std::array<Slot, 32> slots;
        bool push (const juce::MidiMessage& m);
        template <typename F> void popAll (F&& f);
    };
    void timerCallback() override;
    void handleIncomingMidiMessage (juce::MidiInput*, const juce::MidiMessage& m) override { receive (m); }
    void deliver (const juce::MidiMessage& m);
    void handleIncoming (const juce::MidiMessage& m);

    mutable juce::CriticalSection lock;
    std::deque<juce::MidiMessage> queue, hwIncoming;
    std::vector<std::vector<juce::uint8>> dump;
    Ring outRing, inRing;                                             // message thread -> audio thread, audio thread -> message thread
    std::unique_ptr<juce::MidiOutput> hwOut; std::unique_ptr<juce::MidiInput> hwIn;
    juce::String outId, inId, lastRxText;
    std::atomic<int> sent { 0 }, received { 0 }, dropped { 0 };
    bool useHardware = true, usePlugin = true, mirror = false, capturing = false, wireAware = true;
    int holdTicks = 0;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (QuadraVerbSysEx)
};
