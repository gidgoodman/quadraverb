#pragma once
#include <juce_audio_utils/juce_audio_utils.h>
#include <unordered_map>
#include <string>
#include <array>
#include "Engine.h"
#include "Params.h"
#include "Presets.h"
#include "QuadraVerbSysEx.h"
#include "SysExMap.h"

class QuadraProcessor : public juce::AudioProcessor, private juce::AsyncUpdater
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
    bool acceptsMidi() const override { return true; }
    bool producesMidi() const override { return true; }   // SysEx out (the hardware path in the SysEx panel avoids hosts that drop it)
    double getTailLengthSeconds() const override { return 12.0; }
    int getNumPrograms() override { return kNumPresets; }
    int getCurrentProgram() override { return getProgram(); }
    void setCurrentProgram (int i) override { changeProgram (i); }
    const juce::String getProgramName (int i) override { return slotName (i); }
    void changeProgramName (int, const juce::String&) override {}
    void getStateInformation (juce::MemoryBlock&) override;
    void setStateInformation (const void*, int) override;

    // ---- programs: 00-99 start as the factory programs; STORE puts your own over any slot (kept in a shared file too)
    void loadPreset (int index);          // applies a program at once (state restore, tests)
    void changeProgram (int index);       // click-free: fades the output, swaps the program while silent, clears the effect tails, fades back up
    void storeProgram (int slot);
    void recallFactory (int slot);
    void recallAllFactory();
    juce::String currentName() const;
    void setCurrentName (const juce::String& n);
    juce::String slotName (int slot) const;
    bool slotIsUser (int slot) const;
    int  getProgram() const { return (int) apvts.state.getProperty ("program", 0); }

    // ---- sampling configuration (front panel): BYPASS records, EQ plays; the engine reports its state back for the LCD
    void requestRecord() { recRequest = 1; }
    void requestPlay()   { playRequest = 1; }
    int  sampleState() const   { return sampState.load(); }    // 0 idle, 1 waiting for audio, 2 recording
    bool samplePlaying() const { return sampPlaying.load(); }

    QuadraVerbSysEx sysex;                       // throttled SysEx output, hardware MIDI ports, dump storage
    void sysexMirror (const char* paramId);      // sends a parameter packet if mirroring is on and the parameter has a page number

    juce::AudioProcessorValueTreeState apvts;
    std::atomic<float> inMeter { 0.f }, outMeter { 0.f };

private:
    static juce::AudioProcessorValueTreeState::ParameterLayout makeLayout();
    qv::EP readParams();
    float rawf (const char* id) const { return rp[(size_t) pidx.at (id)]->load(); }
    juce::ValueTree userNode (int slot) const;
    static juce::File userBankFile();
    void loadUserBank();
    void saveUserBank() const;
    void handleAsyncUpdate() override;

    qv::Engine engine;
    std::vector<std::atomic<float>*> rp;
    std::unordered_map<std::string, int> pidx;
    std::atomic<int> pendingProgram { -1 };
    // program change muting (audio thread owns muteGain)
    std::atomic<bool> muteTarget { false }, resetPending { false };
    float muteGain = 1.f, lastInGain = 1.f, lastOutGain = 1.f; double sampleRateNow = 48000.0;
    struct ProgramTimer : juce::Timer { std::function<void()> fn; void timerCallback() override { stopTimer(); if (fn) fn(); } } programTimer;
    std::atomic<int> programTarget { -1 };
    std::atomic<int> recRequest { 0 }, playRequest { 0 }, sampState { 0 };
    std::atomic<bool> sampPlaying { false };
    float cc[128] = {}, bend = 0.f, aftertouch = 0.f, noteNum = 0.f, noteVel = 0.f;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (QuadraProcessor)
};
