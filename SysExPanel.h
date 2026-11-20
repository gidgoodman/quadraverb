#pragma once
#include "QuadraVerbSysEx.h"

// Overlay on the faceplate: hardware MIDI In/Out selection next to the plugin's own MIDI output, dump capture,
// .syx import/export and a raw packet sender for probing the unit. Opened with the faceplate's POWER button.
class SysExPanel : public juce::Component, private juce::Timer
{
public:
    explicit SysExPanel (QuadraVerbSysEx& s);
    ~SysExPanel() override { setLookAndFeel (nullptr); }
    std::function<void()> onClose;
    void paint (juce::Graphics&) override;
    void resized() override;
    void visibilityChanged() override { if (isVisible()) refreshDevices(); }

private:
    void refreshDevices();
    void timerCallback() override;
    void chooseImport();
    void chooseExport();
    void sendTest();

    juce::LookAndFeel_V4 panelLaf;   // own look: the faceplate's look-and-feel draws buttons as invisible hit zones
    QuadraVerbSysEx& sx;
    juce::Label outL { {}, "MIDI OUT" }, inL { {}, "MIDI IN" }, status, funcL { {}, "FUNC" }, pageL { {}, "PAGE" }, valL { {}, "VALUE" };
    juce::ComboBox outBox, inBox;
    juce::ToggleButton hwToggle { "Hardware out" }, plugToggle { "Plugin MIDI out" }, mirrorToggle { "Mirror panel edits" }, captureToggle { "Capture dump" };
    juce::TextButton sendDumpBtn { "Send dump" }, importBtn { "Import .syx" }, exportBtn { "Export .syx" }, clearBtn { "Clear" }, rescanBtn { "Rescan" }, closeBtn { "X" }, testBtn { "Send" };
    juce::TextEditor funcEd, pageEd, valEd;
    juce::StringArray outIds, inIds;
    std::unique_ptr<juce::FileChooser> chooser;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SysExPanel)
};
