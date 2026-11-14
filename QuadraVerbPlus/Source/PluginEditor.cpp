#include "PluginEditor.h"

static const juce::Colour kRed (0xffff2a1a);

QuadraEditor::QuadraEditor (QuadraProcessor& p) : AudioProcessorEditor (&p), proc (p)
{
    setLookAndFeel (&laf);
    addKnob (inK,  "in",  "INPUT");
    addKnob (mixK, "mix", "MIX");
    addKnob (outK, "out", "OUTPUT");
    for (int i = 0; i < 8; ++i) addKnob (edit[(size_t) i], kEditIds[i], kEditNames[i]);

    for (auto* b : { &prev, &next })
    {
        b->setColour (juce::TextButton::buttonColourId, juce::Colour (0xff2c2c2e));
        b->setColour (juce::TextButton::textColourOffId, juce::Colours::white);
        addAndMakeVisible (*b);
    }
    prev.onClick = [this] { proc.loadPreset (proc.getProgram() - 1); };
    next.onClick = [this] { proc.loadPreset (proc.getProgram() + 1); };

    setSize (1100, 200);
    startTimerHz (30);
}

void QuadraEditor::addKnob (Knob& k, const juce::String& id, const juce::String& name)
{
    k.name = name;
    k.s.setSliderStyle (juce::Slider::RotaryVerticalDrag);
    k.s.setTextBoxStyle (juce::Slider::NoTextBox, true, 0, 0);
    k.s.setRotaryParameters (juce::degreesToRadians (225.f), juce::degreesToRadians (495.f), true);
    addAndMakeVisible (k.s);
    k.att = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment> (proc.apvts, id, k.s);
}

void QuadraEditor::resized()
{
    inK.s.setBounds  (46, 44, 66, 66);
    mixK.s.setBounds (884, 44, 66, 66);
    outK.s.setBounds (974, 44, 66, 66);
    for (int i = 0; i < 8; ++i)
        edit[(size_t) i].s.setBounds (508 + (i % 4) * 90, 24 + (i / 4) * 84, 44, 44);
    prev.setBounds (400, 152, 34, 26);
    next.setBounds (440, 152, 34, 26);
}

void QuadraEditor::drawMeter (juce::Graphics& g, juce::Rectangle<int> r, float level)
{
    const float db = juce::Decibels::gainToDecibels (level, -60.f);
    const int lit = juce::jlimit (0, 8, (int) ((db + 48.f) / 6.f));
    const int segH = r.getHeight() / 8;
    for (int i = 0; i < 8; ++i)
    {
        auto seg = juce::Rectangle<int> (r.getX(), r.getBottom() - (i + 1) * segH + 1, r.getWidth(), segH - 2);
        auto col = i >= 7 ? juce::Colour (0xffff3020) : i >= 5 ? juce::Colour (0xffffc020) : juce::Colour (0xff30e050);
        g.setColour (i < lit ? col : col.withAlpha (0.12f));
        g.fillRect (seg);
    }
}

void QuadraEditor::paint (juce::Graphics& g)
{
    auto b = getLocalBounds().toFloat();
    g.setGradientFill (juce::ColourGradient (juce::Colour (0xff2a2a2c), 0, 0, juce::Colour (0xff111113), 0, b.getHeight(), false));
    g.fillAll();
    // brushed texture
    juce::Random rnd (7);
    for (int y = 0; y < getHeight(); y += 2)
    { g.setColour (juce::Colours::white.withAlpha (rnd.nextFloat() * 0.025f)); g.drawHorizontalLine (y, 0, b.getWidth()); }

    // rack ears + screws
    g.setColour (juce::Colour (0xff0b0b0c)); g.fillRect (0, 0, 26, getHeight()); g.fillRect (getWidth() - 26, 0, 26, getHeight());
    for (float sy : { 28.f, 172.f })
        for (float sx : { 13.f, (float) getWidth() - 13.f })
        {
            g.setColour (juce::Colour (0xff8a8a8c)); g.fillEllipse (sx - 6, sy - 6, 12, 12);
            g.setColour (juce::Colour (0xff222222)); g.drawLine (sx - 4, sy - 1, sx + 4, sy + 2, 1.5f);
        }
    g.setColour (juce::Colours::white.withAlpha (0.12f)); g.drawRect (getLocalBounds(), 1);

    // knob labels
    g.setFont (juce::FontOptions (11.f, juce::Font::bold));
    g.setColour (juce::Colour (0xffd8d8d8));
    auto label = [&] (Knob& k, int dy) {
        g.drawText (k.name, k.s.getBounds().withY (k.s.getBottom() + dy).withHeight (14).expanded (20, 0),
                    juce::Justification::centred); };
    label (inK, 0); label (mixK, 0); label (outK, 0);
    for (auto& k : edit) label (k, -2);

    // section headings
    g.setColour (juce::Colour (0xff9a9a9c)); g.setFont (juce::FontOptions (10.f, juce::Font::bold));
    g.drawText ("EDIT PARAMETERS", 500, 6, 360, 12, juce::Justification::centred);
    g.drawLine (506, 14, 540, 14); g.drawLine (820, 14, 854, 14);
    g.drawText ("IN", meterIn.getX() - 6, meterIn.getBottom() + 4, 24, 12, juce::Justification::centred);
    g.drawText ("OUT", meterOut.getX() - 6, meterOut.getBottom() + 4, 24, 12, juce::Justification::centred);

    drawMeter (g, meterIn, proc.inMeter.load());
    drawMeter (g, meterOut, proc.outMeter.load());

    // LED display
    g.setColour (juce::Colour (0xff050000)); g.fillRoundedRectangle (display.toFloat(), 4.f);
    g.setColour (juce::Colour (0xff3a3a3c)); g.drawRoundedRectangle (display.toFloat().expanded (1), 5.f, 2.f);
    const int prog = proc.getProgram();
    auto mono = juce::Font (juce::FontOptions (juce::Font::getDefaultMonospacedFontName(), 28.f, juce::Font::bold));
    auto txt = [&] (const juce::String& s, juce::Rectangle<int> r, juce::Font f) {
        g.setFont (f);
        g.setColour (kRed.withAlpha (0.18f)); g.drawText (s, r.expanded (1, 1), juce::Justification::centredLeft);
        g.setColour (kRed); g.drawText (s, r, juce::Justification::centredLeft); };
    txt ("PGM " + juce::String (prog + 1).paddedLeft ('0', 2), display.withTrimmedLeft (14).withHeight (40), mono);
    txt (kPresets[prog].name, display.withTrimmedLeft (14).withTrimmedTop (36).withHeight (32), mono.withHeight (20.f));

    auto on = [&] (const char* n, bool lit, int x) {
        g.setFont (juce::FontOptions (11.f, juce::Font::bold));
        g.setColour (lit ? kRed : kRed.withAlpha (0.15f));
        g.drawText (n, display.getX() + x, display.getBottom() - 24, 60, 14, juce::Justification::left); };
    auto v = [&] (const char* id) { return proc.apvts.getRawParameterValue (id)->load(); };
    on ("REVERB", v ("size") > 0.02f, 14);
    on ("CHORUS", v ("cdepth") > 0.02f, 88);
    on ("DELAY",  v ("dfb") > 0.02f, 162);
    on ("EQ", std::abs (v ("low")) + std::abs (v ("high")) > 0.5f, 232);

    // title
    g.setColour (juce::Colours::white); g.setFont (juce::FontOptions (22.f, juce::Font::bold));
    g.drawText ("QUADRAVERB PLUS", 160, 142, 230, 26, juce::Justification::left);
    g.setColour (juce::Colour (0xff9a9a9c)); g.setFont (juce::FontOptions (10.f));
    g.drawText ("4-CHANNEL DIGITAL EFFECTS PROCESSOR", 160, 166, 230, 14, juce::Justification::left);
}
