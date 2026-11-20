#pragma once
#include "PluginProcessor.h"
#include "SysExPanel.h"

// Draws the knob caps over the photographed knobs; buttons are invisible hit zones
struct PanelLAF : juce::LookAndFeel_V4
{
    void drawRotarySlider (juce::Graphics& g, int x, int y, int w, int h, float pos,
                           float a0, float a1, juce::Slider&) override
    {
        const auto c = juce::Rectangle<float> ((float) x, (float) y, (float) w, (float) h).getCentre();
        const float r = (float) juce::jmin (w, h) / 2.f;
        g.setColour (juce::Colours::black.withAlpha (0.55f)); g.fillEllipse (c.x - r, c.y - r + 0.06f * r, 2 * r, 2 * r);   // soft shadow
        g.setGradientFill (juce::ColourGradient (juce::Colour (0xff4a4a4d), c.x - r * 0.45f, c.y - r * 0.55f,
                                                 juce::Colour (0xff0a0a0b), c.x + r * 0.7f, c.y + r * 0.8f, true));
        g.fillEllipse (c.x - r * 0.96f, c.y - r * 0.96f, r * 1.92f, r * 1.92f);                                              // glossy black body
        g.setColour (juce::Colour (0xff1c1c1e)); g.drawEllipse (c.x - r * 0.96f, c.y - r * 0.96f, r * 1.92f, r * 1.92f, r * 0.06f);
        juce::Path p; p.addRoundedRectangle (-r * 0.045f, -r * 0.80f, r * 0.09f, r * 0.80f, r * 0.04f);                      // thin white pointer
        g.setColour (juce::Colours::white);
        g.fillPath (p, juce::AffineTransform::rotation (a0 + pos * (a1 - a0)).translated (c));
    }
    void drawButtonBackground (juce::Graphics& g, juce::Button& b, const juce::Colour&, bool hover, bool down) override
    {
        if (down)       { g.setColour (juce::Colours::black.withAlpha (.4f));  g.fillRoundedRectangle (b.getLocalBounds().toFloat(), 2.f); }
        else if (hover) { g.setColour (juce::Colours::white.withAlpha (.07f)); g.fillRoundedRectangle (b.getLocalBounds().toFloat(), 2.f); }
    }
    void drawButtonText (juce::Graphics&, juce::TextButton&, bool, bool) override {}
};

class QuadraEditor : public juce::AudioProcessorEditor, private juce::Timer
{
public:
    explicit QuadraEditor (QuadraProcessor&);
    ~QuadraEditor() override { setLookAndFeel (nullptr); }
    void paint (juce::Graphics&) override;
    void resized() override;
    bool keyPressed (const juce::KeyPress&) override;

private:
    struct Page { const char* id; juce::String title; };
    std::vector<Page> buildPages() const;
    void pressed (int zone);
    void toggleParam (const char* id);
    void stepValue (int dir);
    void doStore();
    float raw (const char* id) const { return proc.apvts.getRawParameterValue (id)->load(); }
    juce::String valueText (const PDef& d, float v) const;
    void timerCallback() override
    {
        level = juce::jmax (proc.inMeter.load(), level * 0.85f);
        const int pg = proc.getProgram(); if (pg != lastProg) { lastProg = pg; edited = false; }   // a newly loaded program is unedited
        repaint();
    }

    QuadraProcessor& proc;
    PanelLAF laf;
    juce::Image face;
    juce::Slider inS, outS;
    SysExPanel sysexPanel { proc.sysex };       // opened with the POWER button
    juce::TextButton powerBtn;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> inAtt, outAtt;
    std::array<juce::TextButton, 16> zone;
    int sel = 0, pageIdx = 0, streak = 0;   // starts on the program display (PROG)
    int storePage = 0, storeDest = 0, recallSrc = 0, nameCursor = 0;
    juce::String msg; juce::uint32 msgUntil = 0;
    void executeStore();
    bool edited = false; int lastProg = -1;
    juce::uint32 lastStep = 0;
    float level = 0.f;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (QuadraEditor)
};
