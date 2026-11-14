#include "PluginProcessor.h"
#include "PluginEditor.h"

using Coeffs = juce::dsp::IIR::Coefficients<float>;

juce::AudioProcessorValueTreeState::ParameterLayout QuadraProcessor::makeLayout()
{
    using R = juce::NormalisableRange<float>;
    std::vector<std::unique_ptr<juce::RangedAudioParameter>> p;
    auto add = [&] (const char* id, const char* n, R r, float d)
    { p.push_back (std::make_unique<juce::AudioParameterFloat> (juce::ParameterID { id, 1 }, n, r, d)); };

    add ("in",     "Input",       R (-24.f, 12.f), 0.f);
    add ("mix",    "Mix",         R (0.f, 1.f),    0.35f);
    add ("out",    "Output",      R (-24.f, 12.f), 0.f);
    add ("size",   "Reverb Size", R (0.f, 1.f),    0.8f);
    add ("damp",   "HF Damping",  R (0.f, 1.f),    0.35f);
    add ("dtime",  "Delay Time",  R (1.f, 1000.f, 0.f, 0.4f), 120.f);
    add ("dfb",    "Delay Fdbk",  R (0.f, 0.95f),  0.f);
    add ("crate",  "Chorus Rate", R (0.05f, 10.f, 0.f, 0.4f), 0.5f);
    add ("cdepth", "Chorus Depth",R (0.f, 1.f),    0.f);
    add ("low",    "Low EQ",      R (-12.f, 12.f), 0.f);
    add ("high",   "High EQ",     R (-12.f, 12.f), 2.f);
    return { p.begin(), p.end() };
}

QuadraProcessor::QuadraProcessor()
    : AudioProcessor (BusesProperties()
          .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
          .withOutput ("Output", juce::AudioChannelSet::stereo(), true)),
      apvts (*this, nullptr, "STATE", makeLayout()) {}

bool QuadraProcessor::isBusesLayoutSupported (const BusesLayout& l) const
{
    return l.getMainOutputChannelSet() == juce::AudioChannelSet::stereo()
        && l.getMainInputChannelSet()  == juce::AudioChannelSet::stereo();
}

void QuadraProcessor::prepareToPlay (double sampleRate, int blockSize)
{
    sr = sampleRate;
    juce::dsp::ProcessSpec spec { sampleRate, (juce::uint32) blockSize, 2 };
    lowShelf.prepare (spec); highShelf.prepare (spec);
    chorus.prepare (spec);
    chorus.setCentreDelay (7.f); chorus.setFeedback (0.2f);
    delay.prepare (spec);
    delay.setMaximumDelayInSamples ((int) (sampleRate * 1.1));
    reverb.prepare (spec);
    dry.setSize (2, blockSize);
}

void QuadraProcessor::processBlock (juce::AudioBuffer<float>& buf, juce::MidiBuffer&)
{
    juce::ScopedNoDenormals noDenormals;
    const int n = buf.getNumSamples(), nCh = juce::jmin (2, buf.getNumChannels());

    buf.applyGain (juce::Decibels::decibelsToGain (param ("in")));
    inMeter = buf.getMagnitude (0, n);

    dry.setSize (2, n, false, false, true);
    for (int c = 0; c < nCh; ++c) dry.copyFrom (c, 0, buf, c, 0, n);

    juce::dsp::AudioBlock<float> block (buf);
    juce::dsp::ProcessContextReplacing<float> ctx (block);

    // --- EQ
    *lowShelf.state  = *Coeffs::makeLowShelf  (sr, 200.f,  0.707f, juce::Decibels::decibelsToGain (param ("low")));
    *highShelf.state = *Coeffs::makeHighShelf (sr, 5000.f, 0.707f, juce::Decibels::decibelsToGain (param ("high")));
    lowShelf.process (ctx); highShelf.process (ctx);

    // --- Chorus / flange
    const float depth = param ("cdepth");
    chorus.setRate (param ("crate"));
    chorus.setDepth (depth);
    chorus.setMix (depth > 0.01f ? 0.5f : 0.f);
    chorus.process (ctx);

    // --- Delay
    delay.setDelay ((float) (param ("dtime") * 0.001 * sr));
    const float fb = param ("dfb");
    for (int c = 0; c < nCh; ++c)
    {
        auto* d = buf.getWritePointer (c);
        for (int i = 0; i < n; ++i)
        {
            const float del = delay.popSample (c);
            delay.pushSample (c, d[i] + del * fb);
            d[i] += del * (fb > 0.01f ? 0.6f : 0.f);
        }
    }

    // --- Reverb
    juce::dsp::Reverb::Parameters rp;
    rp.roomSize = param ("size"); rp.damping = param ("damp");
    rp.wetLevel = 0.5f; rp.dryLevel = 0.5f; rp.width = 1.f; rp.freezeMode = 0.f;
    reverb.setParameters (rp);
    reverb.process (ctx);

    // --- Mix + output
    const float mix = param ("mix"), og = juce::Decibels::decibelsToGain (param ("out"));
    for (int c = 0; c < nCh; ++c)
    {
        auto* w = buf.getWritePointer (c); auto* dr = dry.getReadPointer (c);
        for (int i = 0; i < n; ++i) w[i] = (dr[i] * (1.f - mix) + w[i] * mix) * og;
    }
    outMeter = buf.getMagnitude (0, n);
}

void QuadraProcessor::loadPreset (int index)
{
    index = (index + 8) % 8;
    apvts.state.setProperty ("program", index, nullptr);
    for (int i = 0; i < 8; ++i)
        if (auto* p = apvts.getParameter (kEditIds[i]))
            p->setValueNotifyingHost (p->convertTo0to1 (kPresets[index].v[i]));
}

void QuadraProcessor::getStateInformation (juce::MemoryBlock& dest)
{
    if (auto xml = apvts.copyState().createXml()) copyXmlToBinary (*xml, dest);
}
void QuadraProcessor::setStateInformation (const void* data, int size)
{
    if (auto xml = getXmlFromBinary (data, size)) apvts.replaceState (juce::ValueTree::fromXml (*xml));
}

juce::AudioProcessorEditor* QuadraProcessor::createEditor() { return new QuadraEditor (*this); }
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter() { return new QuadraProcessor(); }
