#pragma once
#include <juce_audio_utils/juce_audio_utils.h>
#include <juce_dsp/juce_dsp.h>

// Edit-page parameter ids / labels (order matches presets below)
static const char* kEditIds[8]   = { "size","damp","dtime","dfb","crate","cdepth","low","high" };
static const char* kEditNames[8] = { "REV SIZE","HF DAMP","DLY TIME","DLY FDBK","CHO RATE","CHO DEPTH","LOW EQ","HIGH EQ" };

struct Preset { const char* name; float v[8]; };
static const Preset kPresets[8] = {
    { "PLATE HALL",     { 0.80f, 0.35f, 120.f, 0.00f, 0.5f, 0.00f,  0.f,  2.f } },
    { "GATED ROOM",     { 0.35f, 0.15f,  60.f, 0.00f, 0.5f, 0.00f,  2.f,  3.f } },
    { "CHORUS VERB",    { 0.60f, 0.45f, 100.f, 0.00f, 0.8f, 0.55f,  0.f,  1.f } },
    { "SLAP ECHO",      { 0.25f, 0.50f, 140.f, 0.25f, 0.5f, 0.00f,  0.f,  0.f } },
    { "FLANGE SPACE",   { 0.55f, 0.40f, 300.f, 0.35f, 0.2f, 0.85f,  0.f,  2.f } },
    { "VOCAL BRIGHT",   { 0.45f, 0.20f, 250.f, 0.20f, 0.6f, 0.25f, -3.f,  5.f } },
    { "DARK CATHEDRAL", { 0.98f, 0.75f, 400.f, 0.30f, 0.3f, 0.10f,  3.f, -5.f } },
    { "TAPE DELAY",     { 0.20f, 0.60f, 380.f, 0.55f, 0.4f, 0.15f,  1.f, -3.f } },
};

class QuadraProcessor : public juce::AudioProcessor
{
public:
    QuadraProcessor();
    void prepareToPlay (double sampleRate, int blockSize) override;
    void releaseResources() override {}
    bool isBusesLayoutSupported (const BusesLayout&) const override;
    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override { return true; }
    const juce::String getName() const override { return "QuadraVerb Plus"; }
    bool acceptsMidi() const override { return false; }
    bool producesMidi() const override { return false; }
    double getTailLengthSeconds() const override { return 5.0; }
    int getNumPrograms() override { return 1; }
    int getCurrentProgram() override { return 0; }
    void setCurrentProgram (int) override {}
    const juce::String getProgramName (int) override { return {}; }
    void changeProgramName (int, const juce::String&) override {}
    void getStateInformation (juce::MemoryBlock&) override;
    void setStateInformation (const void*, int) override;

    void loadPreset (int index);
    int  getProgram() const { return (int) apvts.state.getProperty ("program", 0); }

    juce::AudioProcessorValueTreeState apvts;
    std::atomic<float> inMeter { 0.f }, outMeter { 0.f };

private:
    static juce::AudioProcessorValueTreeState::ParameterLayout makeLayout();
    float param (const char* id) const { return apvts.getRawParameterValue (id)->load(); }

    double sr = 44100.0;
    juce::dsp::ProcessorDuplicator<juce::dsp::IIR::Filter<float>, juce::dsp::IIR::Coefficients<float>> lowShelf, highShelf;
    juce::dsp::Chorus<float> chorus;
    juce::dsp::DelayLine<float, juce::dsp::DelayLineInterpolationTypes::Linear> delay { 192000 };
    juce::dsp::Reverb reverb;
    juce::AudioBuffer<float> dry;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (QuadraProcessor)
};
