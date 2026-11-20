#include "PluginProcessor.h"
#include <cmath>

// Forward declaration of editor factory defined in PluginEditor.cpp
juce::AudioProcessorEditor* createQuadraEditor (QuadraProcessor&);

juce::AudioProcessorValueTreeState::ParameterLayout QuadraProcessor::makeLayout()
{
    std::vector<std::unique_ptr<juce::RangedAudioParameter>> params;
    for (int i = 0; i < kNumParams; ++i)
    {
        const PDef& d = kParams[i];
        const juce::ParameterID pid { d.id, 1 };
        switch (d.kind)
        {
            case KF: params.push_back (std::make_unique<juce::AudioParameterFloat> (pid, d.name, juce::NormalisableRange<float> (d.mn, d.mx, d.step), d.def)); break;
            case KI: params.push_back (std::make_unique<juce::AudioParameterInt>   (pid, d.name, (int) d.mn, (int) d.mx, (int) d.def)); break;
            case KC: { juce::StringArray sa; sa.addTokens (d.labels, "|", ""); params.push_back (std::make_unique<juce::AudioParameterChoice> (pid, d.name, sa, (int) d.def)); break; }
            case KB: params.push_back (std::make_unique<juce::AudioParameterBool>  (pid, d.name, d.def > 0.5f)); break;
        }
    }
    return { params.begin(), params.end() };
}

QuadraProcessor::QuadraProcessor()
    : AudioProcessor (BusesProperties()
          .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
          .withOutput ("Output", juce::AudioChannelSet::stereo(), true)),
      apvts (*this, nullptr, "STATE", makeLayout())
{
    initFactoryBank();
    changeProgram (0);
}

void QuadraProcessor::initFactoryBank()
{
    static const char* factoryNames = {
        "LARGE HALL 1", "WARM PLATE", "CHAMBER ROOM", "GATED SNARE", "STEREO CHORUS",
        "DEEP FLANGE", "PING PONG DLY", "SYMPHONIC DET", "LEZLIE ORGAN", "8-TAP RHYTHM"
    };

    for (int i = 0; i < kNumPresets; ++i)
    {
        bank[(size_t) i].name = (i < 10) ? factoryNames[i] : ("USER PATCH " + juce::String (i).paddedLeft ('0', 2));
        bank[(size_t) i].values.resize ((size_t) kNumParams);
        for (int p = 0; p < kNumParams; ++p)
            bank[(size_t) i].values[(size_t) p] = kParams[p].def;

        // Tailor first 10 signature factory presets
        auto setSlot = [&] (const char* id, float v)
        {
            for (int p = 0; p < kNumParams; ++p)
                if (std::strcmp (kParams[p].id, id) == 0)
                    bank[(size_t) i].values[(size_t) p] = v;
        };

        if (i == 0) { setSlot ("rtype", 0); setSlot ("rdecay", 68); setSlot ("rlevel", 85); }
        if (i == 1) { setSlot ("rtype", 1); setSlot ("rdecay", 55); setSlot ("rhf", -25); }
        if (i == 2) { setSlot ("rtype", 3); setSlot ("rdecay", 42); setSlot ("rpre", 25); }
        if (i == 3) { setSlot ("rtype", 1); setSlot ("rgate", 1); setSlot ("rghold", 160); }
        if (i == 4) { setSlot ("pmode", 0); setSlot ("pspeed", 35); setSlot ("pdepth", 65); setSlot ("plevel", 85); setSlot ("rlevel", 20); }
        if (i == 5) { setSlot ("pmode", 3); setSlot ("pspeed", 22); setSlot ("pdepth", 85); setSlot ("pfb", 55); setSlot ("plevel", 85); setSlot ("rlevel", 15); }
        if (i == 6) { setSlot ("dtype", 0); setSlot ("dtl", 340); setSlot ("dfl", 45); setSlot ("dylevel", 85); setSlot ("rlevel", 30); }
        if (i == 7) { setSlot ("pmode", 4); setSlot ("pdet", 14); setSlot ("plevel", 90); setSlot ("rdecay", 60); }
        if (i == 8) { setSlot ("config", 1); setSlot ("pspeed", 75); setSlot ("pdepth", 70); setSlot ("plevel", 90); }
        if (i == 9) { setSlot ("config", 3); setSlot ("dtype", 3); setSlot ("dylevel", 95); setSlot ("rlevel", 35); }
    }
}

void QuadraProcessor::changeProgram (int index)
{
    currentProgram = juce::jlimit (0, kNumPresets - 1, index);
    const auto& slot = bank[(size_t) currentProgram];
    for (int p = 0; p < kNumParams; ++p)
    {
        if (std::strcmp (kParams[p].id, "in") == 0 || std::strcmp (kParams[p].id, "out") == 0 || std::strcmp (kParams[p].id, "bypass") == 0)
            continue;
        if (auto* pr = apvts.getParameter (kParams[p].id))
            pr->setValueNotifyingHost (pr->convertTo0to1 (slot.values[(size_t) p]));
    }
}

void QuadraProcessor::storeProgram (int slot)
{
    const int idx = juce::jlimit (0, kNumPresets - 1, slot);
    currentProgram = idx;
    for (int p = 0; p < kNumParams; ++p)
        bank[(size_t) idx].values[(size_t) p] = apvts.getRawParameterValue (kParams[p].id)->load();
}

void QuadraProcessor::recallFactory (int slot)
{
    initFactoryBank();
    changeProgram (slot);
}

juce::String QuadraProcessor::currentName() const { return bank[(size_t) currentProgram].name; }
void QuadraProcessor::setCurrentName (const juce::String& name) { bank[(size_t) currentProgram].name = name.paddedRight (' ', 14).substring (0, 14).trimEnd(); }
juce::String QuadraProcessor::slotName (int slot) const { return bank[(size_t) juce::jlimit (0, kNumPresets - 1, slot)].name; }
void QuadraProcessor::changeProgramName (int index, const juce::String& newName) { bank[(size_t) juce::jlimit (0, kNumPresets - 1, index)].name = newName; }

bool QuadraProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
    return layouts.getMainOutputChannelSet() == juce::AudioChannelSet::stereo()
        && layouts.getMainInputChannelSet()  == juce::AudioChannelSet::stereo();
}

void QuadraProcessor::prepareToPlay (double sampleRate, int)
{
    sr = juce::jmax (8000.0, sampleRate);
    const int maxDelaySamples = (int) (sr * 2.0) + 16;
    delayLineL.assign ((size_t) maxDelaySamples, 0.f);
    delayLineR.assign ((size_t) maxDelaySamples, 0.f);
    delayWritePos = 0;

    const int combLens = { 1557, 1617, 1491, 1422 };
    const int apLens   = { 225, 556 };
    const double scale = sr / 44100.0;
    for (int i = 0; i < 4; ++i)
    {
        combL[i].assign ((size_t) (combLens[i] * scale), 0.f);
        combR[i].assign ((size_t) ((combLens[i] + 23) * scale), 0.f);
        combPosL[i] = combPosR[i] = 0;
        combLpL[i] = combLpR[i] = 0.f;
    }
    for (int i = 0; i < 2; ++i)
    {
        apL[i].assign ((size_t) (apLens[i] * scale), 0.f);
        apR[i].assign ((size_t) ((apLens[i] + 17) * scale), 0.f);
        apPosL[i] = apPosR[i] = 0;
    }
}

void QuadraProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer&)
{
    juce::ScopedNoDenormals noDenormals;
    const int numSamples = buffer.getNumSamples();
    if (buffer.getNumChannels() < 2 || numSamples == 0) return;

    auto raw = [&] (const char* id) { return apvts.getRawParameterValue (id)->load(); };

    const float inGain  = juce::Decibels::decibelsToGain (raw ("in"));
    const float outGain = juce::Decibels::decibelsToGain (raw ("out"));
    const bool bypass   = raw ("bypass") > 0.5f;

    float peak = 0.f;
    auto* L = buffer.getWritePointer (0);
    auto* R = buffer.getWritePointer (1);

    for (int i = 0; i < numSamples; ++i)
        peak = juce::jmax (peak, std::abs (L[i] * inGain), std::abs (R[i] * inGain));
    inMeter.store (peak);

    if (bypass)
    {
        buffer.applyGain (inGain * outGain);
        return;
    }

    // Update 3-Band EQ coefficients
    const auto loCo = juce::dsp::IIR::Coefficients<float>::makeLowShelf  (sr, juce::jlimit (20.f, 1000.f, raw ("elf")), 0.707f, juce::Decibels::decibelsToGain (raw ("ela")));
    const auto mdCo = juce::dsp::IIR::Coefficients<float>::makePeakFilter(sr, juce::jlimit (200.f, 10000.f, raw ("emf")), 1.f / juce::jmax (0.2f, raw ("embw")), juce::Decibels::decibelsToGain (raw ("ema")));
    const auto hiCo = juce::dsp::IIR::Coefficients<float>::makeHighShelf (sr, juce::jlimit (2000.f, 18000.f, raw ("ehf")), 0.707f, juce::Decibels::decibelsToGain (raw ("eha")));
    *eqLoL.coefficients = *loCo; *eqLoR.coefficients = *loCo;
    *eqMidL.coefficients = *mdCo; *eqMidR.coefficients = *mdCo;
    *eqHiL.coefficients = *hiCo; *eqHiR.coefficients = *hiCo;

    const int cfg       = (int) raw ("config");
    const int dtype     = (int) raw ("dtype");
    const int dSamplesL = juce::jlimit (1, (int) delayLineL.size() - 2, (int) (raw ("dtl") * 0.001f * sr));
    const int dSamplesR = juce::jlimit (1, (int) delayLineR.size() - 2, (int) ((dtype == 1 ? raw ("dtr") : raw ("dtl")) * 0.001f * sr));
    const float fbL     = raw ("dfl") * 0.0095f;
    const float fbR     = (dtype == 1 ? raw ("dfr") : raw ("dfl")) * 0.0095f;

    const float lfoRate = (0.1f + (raw ("pspeed") / 99.f) * 8.0f) / (float) sr;
    const float pDepth  = raw ("pdepth") / 99.f;
    const float pFb     = raw ("pfb") * 0.0085f;
    const float rmFreq  = raw ("rmshift") / (float) sr;

    const float revRoom = 0.72f + (raw ("rdecay") / 99.f) * 0.255f;
    const float revDamp = juce::jlimit (0.05f, 0.85f, -raw ("rhf") / 110.f);
    const bool revGate  = raw ("rgate") > 0.5f;

    const float dryMix  = raw ("dlevel") / 99.f;
    const float fxMast  = raw ("master") / 99.f;
    const float eqMix   = raw ("eqlevel") / 99.f;
    const float pchMix  = raw ("plevel") / 99.f;
    const float dlyMix  = raw ("dylevel") / 99.f;
    const float revMix  = raw ("rlevel") / 99.f;

    const int bufLen = (int) delayLineL.size();

    for (int i = 0; i < numSamples; ++i)
    {
        const float dryL = L[i] * inGain;
        const float dryR = R[i] * inGain;

        // 1. EQ Stage
        float eqL = eqHiL.processSample (eqMidL.processSample (eqLoL.processSample (dryL))) * eqMix;
        float eqR = eqHiR.processSample (eqMidR.processSample (eqLoR.processSample (dryR))) * eqMix;

        // 2. Pitch / Chorus / Ring Mod Stage
        lfoPhase += lfoRate;
        if (lfoPhase >= 1.f) lfoPhase -= 1.f;
        ringPhase += rmFreq;
        if (ringPhase >= 1.f) ringPhase -= 1.f;

        float pchL = 0.f, pchR = 0.f;
        if (cfg == 5) // Ring Modulator
        {
            const float carrier = std::sin (ringPhase * juce::MathConstants<float>::twoPi);
            pchL = eqL * carrier * pchMix;
            pchR = eqR * carrier * pchMix;
        }
        else
        {
