#include "PluginProcessor.h"
#include "PluginEditor.h"

juce::AudioProcessorValueTreeState::ParameterLayout QuadraProcessor::makeLayout()
{
    std::vector<std::unique_ptr<juce::RangedAudioParameter>> p;
    for (int i = 0; i < kNumParams; ++i)
    {
        const PDef& d = kParams[i];
        const juce::ParameterID pid { d.id, 1 };
        switch (d.kind)
        {
            case KF: p.push_back (std::make_unique<juce::AudioParameterFloat> (pid, d.name, juce::NormalisableRange<float> (d.mn, d.mx, d.step), d.def)); break;
            case KI: p.push_back (std::make_unique<juce::AudioParameterInt> (pid, d.name, (int) d.mn, (int) d.mx, (int) d.def)); break;
            case KC: { juce::StringArray sa; sa.addTokens (d.labels, "|", ""); p.push_back (std::make_unique<juce::AudioParameterChoice> (pid, d.name, sa, (int) d.def)); break; }
            case KB: p.push_back (std::make_unique<juce::AudioParameterBool> (pid, d.name, d.def > 0.5f)); break;
        }
    }
    return { p.begin(), p.end() };
}

QuadraProcessor::QuadraProcessor()
    : AudioProcessor (BusesProperties()
          .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
          .withOutput ("Output", juce::AudioChannelSet::stereo(), true)),
      apvts (*this, nullptr, "STATE", makeLayout())
{
    rp.resize ((size_t) kNumParams);   // direct pointers to every parameter, indexed like kParams (used on the audio thread)
    for (int i = 0; i < kNumParams; ++i) { pidx[kParams[i].id] = i; rp[(size_t) i] = apvts.getRawParameterValue (kParams[i].id); }
    jassert (std::all_of (kModTargets, kModTargets + kNumModTargets, [this] (const ModTarget& t) { return pidx.count (t.id) > 0; }));
    sysex.onPacket = [this] (const QuadraVerbSysEx::Packet& p)   // a parameter packet from the unit moves the matching plugin parameter
    {
        if (p.func != QuadraVerbSysEx::kFuncParamSet) return;
        if (const char* id = sysexIdForPage (p.page))
            if (auto* pr = apvts.getParameter (id)) { const auto& d = kParams[pidx.at (id)]; pr->setValueNotifyingHost (pr->convertTo0to1 (juce::jlimit (d.mn, d.mx, d.mn + (float) p.value * d.step))); }
    };
    loadUserBank();
    loadPreset (0);
}

void QuadraProcessor::sysexMirror (const char* id)
{
    if (! sysex.mirrorEnabled()) return;
    const int page = sysexPageFor (id); if (page < 0) return;   // unmapped: nothing is sent (see SysExMap.h)
    const auto& d = kParams[pidx.at (id)];
    sysex.enqueueParam (QuadraVerbSysEx::kFuncParamSet, (juce::uint8) page, (juce::uint16) juce::jlimit (0, 65535, (int) std::lround ((rawf (id) - d.mn) / d.step)));
}

bool QuadraProcessor::isBusesLayoutSupported (const BusesLayout& l) const
{
    return l.getMainOutputChannelSet() == juce::AudioChannelSet::stereo()
        && l.getMainInputChannelSet()  == juce::AudioChannelSet::stereo();
}

void QuadraProcessor::prepareToPlay (double sampleRate, int blockSize)
{
    sampleRateNow = sampleRate;
    engine.prepare (sampleRate, juce::jmax (blockSize, 512));
}

qv::EP QuadraProcessor::readParams()
{
    qv::EP e;
    // MIDI modulation: each active slot offsets its target parameter by (source x amplitude x parameter range)
    std::array<float, kNumParams> off; off.fill (0.f);
    for (int k = 0; k < 8; ++k)
    {
        const int amp = (int) std::lround (rawf (kModIds[k][2])); if (amp == 0) continue;
        const int tgt = juce::jlimit (0, kNumModTargets - 1, (int) std::lround (rawf (kModIds[k][1])));
        const int s = (int) std::lround (rawf (kModIds[k][0]));
        float v = 0.f;
        if (s == 0) v = bend; else if (s == 1) v = aftertouch; else if (s == 2) v = noteNum; else if (s == 3) v = noteVel;
        else if (s <= 131) v = cc[s - 4]; else v = rawf (kHostIds[juce::jlimit (0, 7, s - 132)]) / 127.f;
        const int pi = pidx.at (kModTargets[tgt].id);
        off[(size_t) pi] += v * (float) amp / 99.f * (kParams[pi].mx - kParams[pi].mn);
    }
    auto f = [&] (const char* id)
    {
        const int pi = pidx.at (id);
        return juce::jlimit (kParams[pi].mn, kParams[pi].mx, rp[(size_t) pi]->load() + off[(size_t) pi]);
    };
    auto i = [&] (const char* id) { return (int) std::lround (f (id)); };
    e.config = i ("config");
    e.rtype = i ("rtype"); e.rin1 = i ("rin1"); e.rin2 = i ("rin2"); e.rinmix = i ("rinmix"); e.rpre = i ("rpre"); e.rpremix = i ("rpremix");
    e.rdecay = i ("rdecay"); e.rdiff = i ("rdiff"); e.rdens = i ("rdens"); e.rlf = i ("rlf"); e.rhf = i ("rhf");
    e.rgate = f ("rgate") > 0.5f; e.rghold = i ("rghold"); e.rgrel = i ("rgrel"); e.rglevel = i ("rglevel");
    e.dtype = i ("dtype"); e.din1 = i ("din1"); e.dinmix = i ("dinmix"); e.dtl = i ("dtl"); e.dfl = i ("dfl"); e.dtr = i ("dtr"); e.dfr = i ("dfr");
    e.pmode = i ("pmode"); e.pin = i ("pin"); e.pwave = i ("pwave"); e.pspeed = i ("pspeed"); e.pdepth = i ("pdepth"); e.pfb = i ("pfb");
    e.ptrig = f ("ptrig") > 0.5f; e.pdet = i ("pdet");
    e.psync = i ("psync"); e.dsync = i ("dsync"); e.dsyncr = i ("dsyncr");
    e.elf = f ("elf"); e.ela = f ("ela"); e.emf = f ("emf"); e.embw = f ("embw"); e.ema = f ("ema"); e.ehf = f ("ehf"); e.eha = f ("eha");
    e.elmf = f ("elmf"); e.elmbw = f ("elmbw"); e.elma = f ("elma"); e.ehmf = f ("ehmf"); e.ehmbw = f ("ehmbw"); e.ehma = f ("ehma");
    static const char* gid[11] = { "g0","g1","g2","g3","g4","g5","g6","g7","g8","g9","g10" };
    for (int k = 0; k < 11; ++k) e.g[k] = f (gid[k]);
    e.lzsep = i ("lzsep"); e.lzspeed = i ("lzspeed"); e.lzhigh = i ("lzhigh"); e.lzout = i ("lzout"); e.lzmotor = f ("lzmotor") > 0.5f;
    e.rmshift = i ("rmshift"); e.rmmix = i ("rmmix"); e.rmdr = i ("rmdr"); e.resgate = i ("resgate"); e.resdecay = i ("resdecay");
    for (int k = 0; k < 5; ++k) e.res[k] = i (kResIds[k]);
    for (int k = 0; k < 8; ++k) { e.tapt[k] = i (kTapIds[k][0]); e.tapv[k] = i (kTapIds[k][1]); e.tapp[k] = i (kTapIds[k][2]); e.tapf[k] = i (kTapIds[k][3]); }
    e.mtmaster = i ("mtmaster"); e.ptdepth = i ("ptdepth"); e.ptspeed = i ("ptspeed");
    e.smode = i ("smode"); e.sstart = i ("sstart"); e.slen = i ("slen"); e.sauto = f ("sauto") > 0.5f; e.smidi = i ("smidi");
    e.slow = i ("slow"); e.sbase = i ("sbase"); e.shigh = i ("shigh"); e.slevel = i ("slevel");
    if (e.config != 3 && e.dtype > 2) e.dtype = 2;                        // multi tap delay exists only in the 5-band configuration
    e.dsel = i ("dsel"); if (e.config != 0 && e.config != 3 && e.dsel > 1) e.dsel = 1;   // panning / tremolo only where the manual has them
    e.dlevel = i ("dlevel"); e.master = i ("master"); e.eqlevel = i ("eqlevel");
    e.plevel = i ("plevel"); e.dylevel = i ("dylevel"); e.rlevel = i ("rlevel");
    return e;
}

void QuadraProcessor::processBlock (juce::AudioBuffer<float>& buf, juce::MidiBuffer& midi)
{
    juce::ScopedNoDenormals noDenormals;
    const int n = buf.getNumSamples();
    if (buf.getNumChannels() < 2) return;

    // MIDI in: controllers, pitch bend, aftertouch, notes feed the MOD sources; program change switches programs
    const int chan = (int) std::lround (rawf ("midich")); const bool pcOn = rawf ("midipc") > 0.5f;
    qv::EP::NoteEv notes[16]; int numNotes = 0;
    for (const auto meta : midi)
    {
        const auto m = meta.getMessage();
        if (m.isSysEx()) { sysex.pushHostSysEx (m); continue; }   // SysEx from the host goes to the message thread (capture / parameter packets)
        if (chan > 0 && m.getChannel() != chan) continue;
        if (m.isController()) cc[m.getControllerNumber() & 127] = (float) m.getControllerValue() / 127.f;
        else if (m.isPitchWheel()) bend = juce::jlimit (-1.f, 1.f, ((float) m.getPitchWheelValue() - 8192.f) / 8192.f);
        else if (m.isChannelPressure()) aftertouch = (float) m.getChannelPressureValue() / 127.f;
        else if (m.isNoteOn()) { noteNum = (float) m.getNoteNumber() / 127.f; noteVel = (float) m.getVelocity() / 127.f; if (numNotes < 16) notes[numNotes++] = { 1, m.getNoteNumber(), m.getVelocity() }; }
        else if (m.isNoteOff()) { if (numNotes < 16) notes[numNotes++] = { 0, m.getNoteNumber(), 0 }; }
        else if (pcOn && m.isProgramChange()) { pendingProgram = m.getProgramChangeNumber(); triggerAsyncUpdate(); }
    }

    sysex.drainOutputTo (midi);   // queued SysEx leaves through the plugin MIDI output, even while bypassed

    if (apvts.getRawParameterValue ("bypass")->load() > 0.5f)
    {
        inMeter = outMeter = buf.getMagnitude (0, n);
        return;
    }
    // a program change that has been applied while the output was held at zero: empty the tails before fading back up
    if (resetPending.exchange (false)) { engine.clear(); muteGain = 0.f; }

    const float inGain = juce::Decibels::decibelsToGain (apvts.getRawParameterValue ("in")->load());
    buf.applyGainRamp (0, n, lastInGain, inGain); lastInGain = inGain;
    inMeter = buf.getMagnitude (0, n);

    qv::EP ep = readParams();
    if (auto* ph = getPlayHead())   // host tempo and bar position for the synced LFO / delays
        if (auto pos = ph->getPosition())
        {
            if (auto b = pos->getBpm()) ep.bpm = juce::jlimit (20.0, 400.0, *b);
            if (auto q = pos->getPpqPosition()) { ep.ppq = *q; ep.ppqValid = true; }
            ep.hostPlaying = pos->getIsPlaying();
        }
    for (int k = 0; k < numNotes; ++k) ep.notes[k] = notes[k];
    ep.numNotes = numNotes; ep.recCmd = recRequest.exchange (0); ep.playCmd = playRequest.exchange (0);
    engine.process (buf.getWritePointer (0), buf.getWritePointer (1), n, ep);
    sampState = engine.sampState; sampPlaying = engine.sampPlaying;

    {   // fade out (5 ms) while a program change is pending, fade back in (20 ms) afterwards
        const float dn = 1.f / (0.005f * (float) sampleRateNow), up = 1.f / (0.020f * (float) sampleRateNow); const bool mute = muteTarget.load();
        for (int i = 0; i < n; ++i)
        {
            muteGain = mute ? juce::jmax (0.f, muteGain - dn) : juce::jmin (1.f, muteGain + up);
            buf.getWritePointer (0)[i] *= muteGain; buf.getWritePointer (1)[i] *= muteGain;
        }
    }
    const float outGain = juce::Decibels::decibelsToGain (apvts.getRawParameterValue ("out")->load());
    buf.applyGainRamp (0, n, lastOutGain, outGain); lastOutGain = outGain;
    outMeter = buf.getMagnitude (0, n);
}

juce::File QuadraProcessor::userBankFile()
{
    return juce::File::getSpecialLocation (juce::File::userApplicationDataDirectory).getChildFile ("QuadraVerb").getChildFile ("UserPrograms.xml");
}
void QuadraProcessor::saveUserBank() const   // the hardware has one program memory; keep ours shared between projects
{
    auto bank = apvts.state.getChildWithName ("USERBANK");
    const auto f = userBankFile();
    if (! bank.isValid() || bank.getNumChildren() == 0) { f.deleteFile(); return; }
    if (auto xml = bank.createXml()) { f.getParentDirectory().createDirectory(); xml->writeTo (f); }
}
void QuadraProcessor::loadUserBank()
{
    if (apvts.state.getChildWithName ("USERBANK").isValid()) return;
    const auto f = userBankFile();
    if (! f.existsAsFile()) return;
    if (auto xml = juce::XmlDocument::parse (f))
    {
        auto bank = juce::ValueTree::fromXml (*xml);
        if (bank.isValid() && bank.hasType ("USERBANK")) apvts.state.appendChild (bank, nullptr);
    }
}

void QuadraProcessor::handleAsyncUpdate()
{
    const int p = pendingProgram.exchange (-1);
    if (p >= 0) changeProgram (p);
}

juce::ValueTree QuadraProcessor::userNode (int slot) const
{
    auto bank = apvts.state.getChildWithName ("USERBANK");
    return bank.isValid() ? bank.getChildWithProperty ("slot", slot) : juce::ValueTree();
}
bool QuadraProcessor::slotIsUser (int slot) const { return userNode (slot).isValid(); }
juce::String QuadraProcessor::slotName (int slot) const
{
    slot = juce::jlimit (0, kNumPresets - 1, slot);
    auto n = userNode (slot);
    return n.isValid() ? n["name"].toString() : juce::String (kPresets[slot].name);
}
juce::String QuadraProcessor::currentName() const
{
    return apvts.state.getProperty ("pname", slotName (getProgram())).toString();
}
void QuadraProcessor::setCurrentName (const juce::String& n)
{
    apvts.state.setProperty ("pname", n.substring (0, 14), nullptr);
}

void QuadraProcessor::loadPreset (int index)
{
    index = ((index % kNumPresets) + kNumPresets) % kNumPresets;
    apvts.state.setProperty ("program", index, nullptr);
    auto set = [this] (const char* id, float v)
    { if (auto* p = apvts.getParameter (id)) p->setValueNotifyingHost (p->convertTo0to1 (v)); };
    for (int i = 0; i < kNumParams; ++i)   // every program starts from the factory defaults
        if (! isGlobalParam (kParams[i].id)) set (kParams[i].id, kParams[i].def);
    auto node = userNode (index);
    if (node.isValid())                    // a stored user program overrides the factory one
    {
        for (int i = 0; i < kNumParams; ++i)
            if (! isGlobalParam (kParams[i].id) && node.hasProperty (juce::Identifier (kParams[i].id)))
                set (kParams[i].id, (float) (double) node[juce::Identifier (kParams[i].id)]);
        apvts.state.setProperty ("pname", node["name"].toString(), nullptr);
    }
    else
    {
        const Preset& pr = kPresets[index];
        for (int i = 0; i < pr.n; ++i) set (pr.pv[i].id, pr.pv[i].v);
        apvts.state.setProperty ("pname", juce::String (pr.name), nullptr);
    }
}

void QuadraProcessor::changeProgram (int index)
{
    if (! juce::MessageManager::getInstance()->isThisTheMessageThread()) { loadPreset (index); return; }
    programTarget = index;
    muteTarget = true;                                   // the audio thread starts a 5 ms fade-out
    programTimer.fn = [this]
    {
        const int i = programTarget.exchange (-1);
        if (i >= 0) loadPreset (i);                      // swap the program while the output is silent...
        resetPending = true;                             // ...clear the old tails...
        muteTarget = false;                              // ...and fade back in
    };
    programTimer.startTimer (14);
}

void QuadraProcessor::storeProgram (int slot)
{
    slot = ((slot % kNumPresets) + kNumPresets) % kNumPresets;
    auto bank = apvts.state.getOrCreateChildWithName ("USERBANK", nullptr);
    auto old = bank.getChildWithProperty ("slot", slot); if (old.isValid()) bank.removeChild (old, nullptr);
    juce::ValueTree node ("P");
    node.setProperty ("slot", slot, nullptr);
    node.setProperty ("name", currentName(), nullptr);
    for (int i = 0; i < kNumParams; ++i)
        if (! isGlobalParam (kParams[i].id)) node.setProperty (juce::Identifier (kParams[i].id), (double) rawf (kParams[i].id), nullptr);
    bank.appendChild (node, nullptr);
    apvts.state.setProperty ("program", slot, nullptr);
    saveUserBank();
}

void QuadraProcessor::recallFactory (int slot)
{
    slot = ((slot % kNumPresets) + kNumPresets) % kNumPresets;
    auto bank = apvts.state.getChildWithName ("USERBANK");
    if (bank.isValid()) { auto n = bank.getChildWithProperty ("slot", slot); if (n.isValid()) bank.removeChild (n, nullptr); }
    saveUserBank();
    loadPreset (slot);
}

void QuadraProcessor::recallAllFactory()
{
    auto bank = apvts.state.getChildWithName ("USERBANK");
    if (bank.isValid())
        for (int i = bank.getNumChildren(); --i >= 0;)
            if ((int) bank.getChild (i)["slot"] < 90) bank.removeChild (i, nullptr);   // the 90 Alesis programs; 90-99 stay yours
    saveUserBank();
    loadPreset (getProgram());
}

void QuadraProcessor::getStateInformation (juce::MemoryBlock& dest)
{
    sysex.saveState (apvts.state);   // the raw SysEx dump travels with the project
    if (auto xml = apvts.copyState().createXml()) copyXmlToBinary (*xml, dest);   // includes the stored programs
}
void QuadraProcessor::setStateInformation (const void* data, int size)
{
    if (auto xml = getXmlFromBinary (data, size))
    {
        apvts.replaceState (juce::ValueTree::fromXml (*xml));
        loadUserBank();   // a project saved without stored programs still picks up the shared bank
        sysex.loadState (apvts.state);
    }
}

juce::AudioProcessorEditor* QuadraProcessor::createEditor() { return new QuadraEditor (*this); }
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter() { return new QuadraProcessor(); }
