#pragma once
#include "PluginProcessor.h"

// Black skirted knob with white indicator line, like the hardware
struct KnobLAF : juce::LookAndFeel_V4
{
    void drawRotarySlider (juce::Graphics& g, int x, int y, int w, int h, float pos,
                           float a0, float a1, juce::Slider&) override
    {
        auto b = juce::Rectangle<float> ((float) x, (float) y, (float) w, (float) h).reduced (3);
        const float r = juce::jmin (b.getWidth(), b.getHeight()) / 2; auto c = b.getCentre();
        g.setColour (juce::Colours::black.withAlpha (0.7f));
        g.fillEllipse (c.x - r - 1, c.y - r + 3, 2 * r + 2, 2 * r + 2);
        g.setGradientFill (juce::ColourGradient (juce::Colour (0xff4a4a4c), c.x - r * 0.6f, c.y - r * 0.6f,
                                                 juce::Colour (0xff0a0a0a), c.x + r, c.y + r, true));
        g.fillEllipse (c.x - r, c.y - r, 2 * r, 2 * r);
        g.setColour (juce::Colour (0xff6a6a6c)); g.drawEllipse (c.x - r, c.y - r, 2 * r, 2 * r, 1.f);
        g.setColour (juce::Colour (0xff111111)); g.drawEllipse (c.x - r * 0.7f, c.y - r * 0.7f, r * 1.4f, r * 1.4f, 1.f);
        juce::Path p; p.addRoundedRectangle (-1.5f, -r + 2.f, 3.f, r * 0.5f, 1.f);
        g.setColour (juce::Colours::white);
        g.fillPath (p, juce::AffineTransform::rotation (a0 + pos * (a1 - a0)).translated (c));
    }
};

class QuadraEditor : public juce::AudioProcessorEditor, private juce::Timer
{
public:
    explicit QuadraEditor (QuadraProcessor&);
    ~QuadraEditor() override { setLookAndFeel (nullptr); }
    void paint (juce::Graphics&) override;
    void resized() override;

private:
    struct Knob {
        juce::Slider s; juce::String name;
        std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> att;
    };
    void addKnob (Knob&, const juce::String& id, const juce::String& name);
    void drawMeter (juce::Graphics&, juce::Rectangle<int>, float level);
    void timerCallback() override { repaint(); }

    QuadraProcessor& proc;
    KnobLAF laf;
    Knob inK, mixK, outK;
    std::array<Knob, 8> edit;
    juce::TextButton prev { "<" }, next { ">" };
    juce::Rectangle<int> display { 160, 26, 320, 104 };
    juce::Rectangle<int> meterIn { 124, 40, 12, 70 }, meterOut { 1058, 40, 12, 70 };

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (QuadraEditor)
};
