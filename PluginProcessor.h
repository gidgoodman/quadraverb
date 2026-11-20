#pragma once
#include <juce_audio_utils/juce_audio_utils.h>
#include <juce_dsp/juce_dsp.h>
#include <array>
#include <vector>
#include "Params.h"

class QuadraProcessor : public juce::AudioProcessor
{
public:
    QuadraProcessor();
    void prepareToPlay (double sampleRate, int blockSize) override;
    void releaseResources() override {}
    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;
    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override { return true; }
    const juce::String getName() const override { return "QuadraVerb Plus"; }
    bool acceptsMidi() const override { return true; }
    bool producesMidi() const override { return true; }
    double getTailLengthSeconds() const override { return 8.0; }

    int getNumPrograms() override { return kNumPresets; }
    int getCurrentProgram() override { return currentProgram; }
    void setCurrentProgram (int index) override { changeProgram (index); }
    const juce::String getProgramName (int index) override { return slotName (index); }
    void changeProgramName (int index, const juce::String& newName) override;

    void getStateInformation (juce::MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;

    void changeProgram (int index);
    void storeProgram (int slot);
    void recallFactory (int slot);
    juce::String currentName() const;
    void setCurrentName (const juce::String& name);
    juce::String slotName (int slot) const;
    int getProgram() const { return currentProgram; }

    juce::AudioProcessorValueTreeState apvts;
    std::atomic<float> inMeter { 0.f };

private:
    static juce::AudioProcessorValueTreeState::ParameterLayout makeLayout();
    void initFactoryBank();

    struct ProgramSlot
    {
        juce::String name;
        std::vector<float> values;
    };

    std::array<ProgramSlot, kNumPresets> bank;
    int currentProgram = 0;

    // DSP State
    double sr = 48000.0;
    juce::dsp::IIR::Filter<float> eqLoL, eqLoR, eqMidL, eqMidR, eqHiL, eqHiR;
    std::vector<float> delayLineL, delayLineR;
    int delayWritePos = 0;
    float lfoPhase = 0.f, ringPhase = 0.f;

    // 4-comb + 2-allpass stereo reverb tank
    std::array<std::vector<float>, 4> combL, combR;
    std::array<int, 4> combPosL {}, combPosR {};
    std::array<float, 4> combLpL {}, combLpR {};
    std::array<std::vector<float>, 2> apL, apR;
    std::array<int, 2> apPosL {}, apPosR {};
    float gateEnv = 1.f;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (QuadraProcessor)
};
