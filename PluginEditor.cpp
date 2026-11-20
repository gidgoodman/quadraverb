#include "PluginEditor.h"
#include "Faceplate.h"

namespace
{
    constexpr int kW = 824, kH = 76;   // panel size in "photo units"; the embedded PNG is 1500x138 (x0.549 into these units)
    enum { PROG, REVERB, DELAY, PITCH, EQ, MIDI, STORE, CONFIG, MIX, MOD, NAME, BYPASS, PAGEUP, PAGEDN, VALUP, VALDN, NZ };
    struct Zone { float cx, cy; const char* name; };
    const Zone kZones[NZ] = {
        { 487.3f, 28.6f, "PROGRAM" }, { 530.7f, 28.6f, "REVERB" }, { 565.3f, 28.6f, "DELAY" }, { 599.9f, 29.1f, "PITCH" },
        { 634.8f, 28.8f, "EQ" }, { 669.4f, 28.6f, "MIDI" }, { 487.8f, 56.3f, "STORE" }, { 530.7f, 56.3f, "CONFIG" },
        { 565.3f, 56.3f, "MIX" }, { 599.9f, 56.3f, "MOD" }, { 634.5f, 56.0f, "NAME" }, { 669.1f, 55.8f, "BYPASS" },
        { 396.1f, 28.6f, "PAGE" }, { 396.1f, 56.3f, "PAGE" }, { 431.2f, 28.6f, "VALUE" }, { 431.2f, 56.3f, "VALUE" } };
    constexpr float kBtnW = 26.f, kBtnH = 10.f;
    const juce::Rectangle<float> kLcd (231.0f, 26.4f, 98.9f, 22.8f);   // the amber LCD window
    constexpr float kLcdTextX = 234.6f, kLcdLine1Y = 30.6f, kLcdLine2Y = 39.6f, kDot = 0.950f;   // 16 x 2 characters of 5x7 dots
    const juce::Colour kLcdTop (0xffeeb21d), kLcdBottom (0xfff0b31e), kInk (0xff3a2a0c);
    const juce::Point<float> kInKnob (125.0f, 43.1f), kOutKnob (170.7f, 43.1f);
    const juce::Point<float> kLeds[4] = { { 85.1f, 37.9f }, { 85.7f, 47.8f }, { 85.1f, 57.1f }, { 85.7f, 66.5f } };

    // classic 5x7 dot-matrix LCD font, ASCII 32-126, 5 column bytes per glyph (bit 0 = top dot)
    static const unsigned char kFont5x7[95 * 5] = {
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x5F, 0x00, 0x00,
        0x00, 0x07, 0x00, 0x07, 0x00, 0x14, 0x7F, 0x14, 0x7F, 0x14,
        0x24, 0x2A, 0x7F, 0x2A, 0x12, 0x23, 0x13, 0x08, 0x64, 0x62,
        0x36, 0x49, 0x56, 0x20, 0x50, 0x00, 0x05, 0x03, 0x00, 0x00,
        0x00, 0x1C, 0x22, 0x41, 0x00, 0x00, 0x41, 0x22, 0x1C, 0x00,
        0x14, 0x08, 0x3E, 0x08, 0x14, 0x08, 0x08, 0x3E, 0x08, 0x08,
        0x00, 0x50, 0x30, 0x00, 0x00, 0x08, 0x08, 0x08, 0x08, 0x08,
        0x00, 0x60, 0x60, 0x00, 0x00, 0x20, 0x10, 0x08, 0x04, 0x02,
        0x3E, 0x51, 0x49, 0x45, 0x3E, 0x00, 0x42, 0x7F, 0x40, 0x00,
        0x42, 0x61, 0x51, 0x49, 0x46, 0x21, 0x41, 0x45, 0x4B, 0x31,
        0x18, 0x14, 0x12, 0x7F, 0x10, 0x27, 0x45, 0x45, 0x45, 0x39,
        0x3C, 0x4A, 0x49, 0x49, 0x30, 0x01, 0x71, 0x09, 0x05, 0x03,
        0x36, 0x49, 0x49, 0x49, 0x36, 0x06, 0x49, 0x49, 0x29, 0x1E,
        0x00, 0x36, 0x36, 0x00, 0x00, 0x00, 0x56, 0x36, 0x00, 0x00,
        0x08, 0x14, 0x22, 0x41, 0x00, 0x14, 0x14, 0x14, 0x14, 0x14,
        0x00, 0x41, 0x22, 0x14, 0x08, 0x02, 0x01, 0x51, 0x09, 0x06,
        0x32, 0x49, 0x79, 0x41, 0x3E, 0x7E, 0x11, 0x11, 0x11, 0x7E,
        0x7F, 0x49, 0x49, 0x49, 0x36, 0x3E, 0x41, 0x41, 0x41, 0x22,
        0x7F, 0x41, 0x41, 0x22, 0x1C, 0x7F, 0x49, 0x49, 0x49, 0x41,
        0x7F, 0x09, 0x09, 0x09, 0x01, 0x3E, 0x41, 0x49, 0x49, 0x7A,
        0x7F, 0x08, 0x08, 0x08, 0x7F, 0x00, 0x41, 0x7F, 0x41, 0x00,
        0x20, 0x40, 0x41, 0x3F, 0x01, 0x7F, 0x08, 0x14, 0x22, 0x41,
        0x7F, 0x40, 0x40, 0x40, 0x40, 0x7F, 0x02, 0x0C, 0x02, 0x7F,
        0x7F, 0x04, 0x08, 0x10, 0x7F, 0x3E, 0x41, 0x41, 0x41, 0x3E,
        0x7F, 0x09, 0x09, 0x09, 0x06, 0x3E, 0x41, 0x51, 0x21, 0x5E,
        0x7F, 0x09, 0x19, 0x29, 0x46, 0x46, 0x49, 0x49, 0x49, 0x31,
        0x01, 0x01, 0x7F, 0x01, 0x01, 0x3F, 0x40, 0x40, 0x40, 0x3F,
        0x1F, 0x20, 0x40, 0x20, 0x1F, 0x3F, 0x40, 0x38, 0x40, 0x3F,
        0x63, 0x14, 0x08, 0x14, 0x63, 0x07, 0x08, 0x70, 0x08, 0x07,
        0x61, 0x51, 0x49, 0x45, 0x43, 0x00, 0x7F, 0x41, 0x41, 0x00,
        0x02, 0x04, 0x08, 0x10, 0x20, 0x00, 0x41, 0x41, 0x7F, 0x00,
        0x04, 0x02, 0x01, 0x02, 0x04, 0x40, 0x40, 0x40, 0x40, 0x40,
        0x00, 0x01, 0x02, 0x04, 0x00, 0x20, 0x54, 0x54, 0x54, 0x78,
        0x7F, 0x48, 0x44, 0x44, 0x38, 0x38, 0x44, 0x44, 0x44, 0x20,
        0x38, 0x44, 0x44, 0x48, 0x7F, 0x38, 0x54, 0x54, 0x54, 0x18,
        0x08, 0x7E, 0x09, 0x01, 0x02, 0x0C, 0x52, 0x52, 0x52, 0x3E,
        0x7F, 0x08, 0x04, 0x04, 0x78, 0x00, 0x44, 0x7D, 0x40, 0x00,
        0x20, 0x40, 0x44, 0x3D, 0x00, 0x7F, 0x10, 0x28, 0x44, 0x00,
        0x00, 0x41, 0x7F, 0x40, 0x00, 0x7C, 0x04, 0x18, 0x04, 0x78,
        0x7C, 0x08, 0x04, 0x04, 0x78, 0x38, 0x44, 0x44, 0x44, 0x38,
        0x7C, 0x14, 0x14, 0x14, 0x08, 0x08, 0x14, 0x14, 0x18, 0x7C,
        0x7C, 0x08, 0x04, 0x04, 0x08, 0x48, 0x54, 0x54, 0x54, 0x20,
        0x04, 0x3F, 0x44, 0x40, 0x20, 0x3C, 0x40, 0x40, 0x20, 0x7C,
        0x1C, 0x20, 0x40, 0x20, 0x1C, 0x3C, 0x40, 0x30, 0x40, 0x3C,
        0x44, 0x28, 0x10, 0x28, 0x44, 0x0C, 0x50, 0x50, 0x50, 0x3C,
        0x44, 0x64, 0x54, 0x4C, 0x44, 0x00, 0x08, 0x36, 0x41, 0x00,
        0x00, 0x00, 0x7F, 0x00, 0x00, 0x00, 0x41, 0x36, 0x08, 0x00,
        0x02, 0x01, 0x02, 0x04, 0x02,
    };

    // Draws one 16-character line of dots. The cell at `cursor` (if any) is drawn inverted.
    void drawDotLine (juce::Graphics& g, const juce::String& text, float x0, float y0, int cursor)
    {
        juce::Path lit, ghost, inv; const float d = kDot * 0.88f;
        for (int ci = 0; ci < 16; ++ci)
        {
            const int gi = juce::jlimit (0, 94, (ci < text.length() ? (int) text[ci] : 32) - 32);
            for (int col = 0; col < 5; ++col)
                for (int row = 0; row < 7; ++row)
                {
                    const juce::Rectangle<float> r (x0 + ((float) ci * 6.f + (float) col) * kDot, y0 + (float) row * kDot, d, d);
                    const bool on = ((kFont5x7[gi * 5 + col] >> row) & 1) != 0;
                    if (ci == cursor) { if (on) inv.addRectangle (r); }
                    else (on ? lit : ghost).addRectangle (r);
                }
        }
        if (cursor >= 0)
        {
            g.setColour (kInk);
            g.fillRect (x0 + (float) cursor * 6.f * kDot - kDot * 0.5f, y0 - kDot * 0.7f, 6.f * kDot, 8.4f * kDot);
            g.setColour (kLcdTop); g.fillPath (inv);
        }
        g.setColour (kInk.withAlpha (0.07f)); g.fillPath (ghost);
        g.setColour (kInk); g.fillPath (lit);
    }

    juce::Rectangle<int> scaled (juce::Rectangle<float> r, float s) { return (r * s).toNearestInt(); }

    const PDef* findDef (const char* id)
    {
        for (int i = 0; i < kNumParams; ++i) if (juce::String (kParams[i].id) == id) return &kParams[i];
        return nullptr;
    }
    juce::StringArray labelsOf (const PDef& d) { juce::StringArray sa; sa.addTokens (d.labels, "|", ""); return sa; }
}

QuadraEditor::QuadraEditor (QuadraProcessor& p) : AudioProcessorEditor (&p), proc (p)
{
    setLookAndFeel (&laf);
    face = juce::ImageFileFormat::loadFrom (kFaceplate, (size_t) kFaceplateSize);

    for (auto* s : { &inS, &outS })
    {
        s->setSliderStyle (juce::Slider::RotaryVerticalDrag);
        s->setTextBoxStyle (juce::Slider::NoTextBox, true, 0, 0);
        s->setRotaryParameters (juce::degreesToRadians (225.f), juce::degreesToRadians (495.f), true);
        addAndMakeVisible (*s);
    }
    inAtt  = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment> (proc.apvts, "in",  inS);
    outAtt = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment> (proc.apvts, "out", outS);

    for (int i = 0; i < NZ; ++i)
    {
        zone[(size_t) i].onClick = [this, i] { pressed (i); };
        addAndMakeVisible (zone[(size_t) i]);
    }
    zone[VALUP].setRepeatSpeed (450, 90);
    zone[VALDN].setRepeatSpeed (450, 90);

    setWantsKeyboardFocus (true);
    addAndMakeVisible (powerBtn);   // the faceplate's POWER button opens the MIDI / SysEx panel
    powerBtn.onClick = [this] { sysexPanel.setVisible (! sysexPanel.isVisible()); if (sysexPanel.isVisible()) sysexPanel.toFront (false); };
    addChildComponent (sysexPanel);
    sysexPanel.onClose = [this] { sysexPanel.setVisible (false); };

    setResizable (true, true);
    setResizeLimits (1000, 92, 2400, 219);
    getConstrainer()->setFixedAspectRatio ((double) kW / kH);
    setSize (1300, 120);
    startTimerHz (30);
}

void QuadraEditor::resized()
{
    const float s = (float) getWidth() / (float) kW;
    for (int i = 0; i < NZ; ++i)
        zone[(size_t) i].setBounds (scaled ({ kZones[i].cx - kBtnW / 2, kZones[i].cy - kBtnH / 2, kBtnW, kBtnH }, s));
    powerBtn.setBounds (scaled ({ 731.2f - 13.f, 18.9f - 5.f, 26.f, 10.f }, s));   // the faceplate POWER button (photo 1331,34.5)
    sysexPanel.setBounds (getLocalBounds());
    inS.setBounds  (scaled ({ kInKnob.x  - 13.f, kInKnob.y  - 13.f, 26, 26 }, s));
    outS.setBounds (scaled ({ kOutKnob.x - 13.f, kOutKnob.y - 13.f, 26, 26 }, s));
}

// The page list for the selected button, in the order the manual describes. Pages vary with configuration and mode.
std::vector<QuadraEditor::Page> QuadraEditor::buildPages() const
{
    std::vector<Page> v; const int cfg = (int) raw ("config");
    auto add = [&] (const char* id, const char* t) { v.push_back ({ id, t }); };
    // configurations: 0 EQ>PCH>DL>REVERB  1 LEZLIE  2 GRAPHIC EQ>DELAY  3 5BAND EQ>PCH>DL  4 EQ>REVERB  5 RING->DL->REVERB  6 RESONATOR>DL>REV  7 SAMPLING
    switch (sel)
    {
        case REVERB:
        {
            if (cfg == 2 || cfg == 3 || cfg == 7) break;
            const int ty = (int) raw ("rtype");
            add ("rtype", "REVERB TYPE:");
            if (cfg != 4) { add ("rin1", "REVERB INPUT 1:"); add ("rin2", "REVERB INPUT 2:"); add ("rinmix", "REVERB INPUT MIX"); }
            add ("rpre", "REVERB PREDELAY:"); add ("rpremix", "PREDELAY MIX:");
            add ("rdecay", ty == 4 ? "REVERSE TIME:" : "REVERB DECAY:");
            if (ty != 4)
            {
                add ("rdiff", "REVERB DIFFUSION"); if (ty != 1) add ("rdens", "REVERB DENSITY:");
                add ("rlf", "LOW FREQ DECAY:"); add ("rhf", "HIGH FREQ DECAY:");
                add ("rgate", "REVERB GATE:"); add ("rghold", "GATE HOLD TIME:"); add ("rgrel", "GATE RELEASE:"); add ("rglevel", "GATED LEVEL:");
            }
            break;
        }
        case DELAY:
        {
            if (cfg == 4) break;
            if (cfg == 7)   // all the sampler's controls live under DELAY, in the order of the addendum
            {
                add ("smode", "SAMPLE PLAYBACK:"); add ("sstart", "SAMPLE START:"); add ("slen", "SAMPLE LENGTH:"); add ("sauto", "AUDIO TRIGGER");
                add ("smidi", "MIDI TRIGGER:"); add ("slow", "MIDI TRIGGER"); add ("sbase", "MIDI TRIGGER"); add ("shigh", "MIDI TRIGGER");
                break;
            }
            const int ty = (int) raw ("dtype");
            add ("dtype", "DELAY TYPE:"); add ("din1", "DELAY INPUT 1:");
            if (cfg != 2) add ("dinmix", cfg == 1 ? "DLY INPUT MIX LZ" : cfg == 5 ? "DLY INPUT MIX RM" : cfg == 6 ? "DLY INPUT MIX RS" : "DELAY INPUT MIX:");
            if (cfg == 3 && ty == 3)   // multi tap: tap number, then that tap's time / volume / panning / feedback, then the master feedback
            {
                const int tap = juce::jlimit (1, 8, (int) raw ("hctap")); const juce::String n (tap);
                v.push_back ({ "hctap", "TAP NUMBER:" });
                v.push_back ({ kTapIds[tap - 1][0], "TAP " + n + " DELAY TIME:" });
                v.push_back ({ kTapIds[tap - 1][1], "TAP " + n + " VOLUME:" });
                v.push_back ({ kTapIds[tap - 1][2], "TAP " + n + " PANNING:" });
                v.push_back ({ kTapIds[tap - 1][3], "TAP " + n + " FEEDBACK:" });
                add ("mtmaster", "MASTER FEEDBACK:");
            }
            else if (ty == 1) { add ("dtl", "LEFT DELAY TIME:"); add ("dsync", "LEFT DELAY SYNC:"); add ("dfl", "DELAY FDBK LEFT:"); add ("dtr", "RIGHT DELAY TIME:"); add ("dsyncr", "RIGHT DLY SYNC:"); add ("dfr", "DELAY FDBK RIGHT"); }
            else { add ("dtl", "DELAY TIME:"); add ("dsync", "DELAY SYNC:"); add ("dfl", "DELAY FEEDBACK:"); }
            break;
        }
        case PITCH:
        {
            if (cfg == 1 || cfg == 2 || cfg == 7) break;
            if (cfg == 5) { add ("rmshift", "SPECTRUM SHIFT:"); add ("rmmix", "RING OUTPUT MIX:"); add ("rmdr", "DLY/REV IN MIX:"); break; }
            if (cfg == 6)
            {
                add ("resgate", "RESONATOR GATE:"); add ("resdecay", "RESONATOR DECAY:");
                for (int k = 0; k < 5; ++k) v.push_back ({ kResIds[k], "RESONATOR " + juce::String (k + 1) + ":" });
                break;
            }
            if (cfg == 4) { add ("pspeed", "REVERB CHR SPEED"); add ("psync", "CHORUS SYNC:"); add ("pdepth", "REVERB CHR DEPTH"); break; }
            const int m = (int) raw ("pmode");
            add ("pmode", "PITCH MODE:"); add ("pin", "PITCH INPUT:");
            if (m == 0 || m == 1) add ("pwave", "LFO WAVESHAPE:");
            if (m != 4) { add ("pspeed", "LFO SPEED:"); add ("psync", "LFO TEMPO SYNC:"); add ("pdepth", "LFO DEPTH:"); }
            if (m <= 3) add ("pfb", "PITCH FEEDBACK:");
            if (m == 2 || m == 3) add ("ptrig", "TRIGGER FLANGE:");
            if (m == 4) add ("pdet", "DETUNE AMOUNT:");
            break;
        }
        case EQ:
        {
            if (cfg == 5 || cfg == 6 || cfg == 7) break;   // (in the sampling configuration this button plays the sample)
            if (cfg == 1) { add ("lzspeed", "LEZLIE SPEED:"); add ("lzmotor", "LEZLIE MOTOR:"); add ("lzsep", "LEZLIE STEREO:"); add ("lzhigh", "HIGH ROTOR LEVEL:"); break; }
            if (cfg == 2)
            {
                static const char* gid[11] = { "g0","g1","g2","g3","g4","g5","g6","g7","g8","g9","g10" };
                static const char* gt[11] = { "16Hz BOOST/CUT:", "32Hz BOOST/CUT:", "62Hz BOOST/CUT:", "126Hz BOOST/CUT:", "250Hz BOOST/CUT:", "500Hz BOOST/CUT:",
                                              "1KHz BOOST/CUT:", "2KHz BOOST/CUT:", "4KHz BOOST/CUT:", "8KHz BOOST/CUT:", "16KHz BOOST/CUT:" };
                for (int i = 0; i < 11; ++i) add (gid[i], gt[i]);
                break;
            }
            add ("elf", "LOW EQ FREQ:"); add ("ela", "LOW EQ AMPLITUDE:");
            if (cfg == 3) { add ("elmf", "LO MID EQ FREQ:"); add ("elmbw", "LO MID EQ BWIDTH"); add ("elma", "LO MID EQ AMP:"); }
            add ("emf", "MID EQ FREQ:"); add ("embw", "MID EQ BANDWIDTH"); add ("ema", "MID EQ AMPLITUDE");
            if (cfg == 3) { add ("ehmf", "HI MID EQ FREQ:"); add ("ehmbw", "HI MID EQ BWIDTH"); add ("ehma", "HI MID EQ AMP:"); }
            add ("ehf", "HI EQ FREQ:"); add ("eha", "HI EQ AMPLITUDE:");
            break;
        }
        case MIX:
            if (cfg == 7) { add ("dlevel", "DIRECT SIGNAL"); add ("slevel", "SAMPLE PLAYBACK"); break; }
            if (cfg == 5 || cfg == 6)
            {
                add ("dlevel", "DIRECT LEVEL:"); add ("master", "MASTER EFFECTS:"); add ("plevel", cfg == 5 ? "RING OUT LEVEL:" : "RESONATOR LEVEL:");
                add ("dylevel", "DELAY OUT LEVEL:"); add ("rlevel", "REVERB OUT LEVEL"); break;
            }
            add ("dsel", "DIRECT SIGNAL:");
            if (raw ("dsel") < 0.5f) add ("dlevel", "DIRECT LEVEL:");
            if ((cfg == 0 || cfg == 3) && raw ("dsel") > 1.5f)   // auto panning / tremolo on the EQ output
            {
                const bool pan = raw ("dsel") < 2.5f;
                add ("ptdepth", pan ? "EQ PANNING DEPTH" : "EQ TREMOLO DEPTH"); add ("ptspeed", pan ? "EQ PANNING SPEED" : "EQ TREMOLO SPEED");
            }
            add ("master", "MASTER EFFECTS:"); add ("eqlevel", "EQ OUTPUT LEVEL:");
            if (cfg == 0 || cfg == 3) add ("plevel", "PITCH OUT LEVEL:");
            if (cfg != 4) add ("dylevel", "DELAY OUT LEVEL:");
            if (cfg == 0 || cfg == 1 || cfg == 4) add ("rlevel", "REVERB OUT LEVEL");
            if (cfg == 1) add ("lzout", "LEZLIE OUT LEVEL");
            break;
        case MOD:
            for (int k = 0; k < 8; ++k)
            {
                const juce::String n (k + 1);
                v.push_back ({ kModIds[k][0], "MOD " + n + " SOURCE:" });
                v.push_back ({ kModIds[k][1], "MOD " + n + " TARGET:" });
                v.push_back ({ kModIds[k][2], "MOD " + n + " AMPLITUDE:" });
            }
            break;
        case MIDI:
            add ("midich", "MIDI CHANNEL:"); add ("midipc", "PROGRAM CHANGE:");
            for (int k = 0; k < 8; ++k) v.push_back ({ kHostIds[k], "HOST CONTROL " + juce::String (k + 1) + ":" });
            break;
        case CONFIG: add ("config", "CONFIGURATION:"); break;
        default: break;
    }
    return v;
}

void QuadraEditor::toggleParam (const char* id)
{
    if (auto* pr = proc.apvts.getParameter (id))
    {
        pr->beginChangeGesture();
        pr->setValueNotifyingHost (pr->getValue() > 0.5f ? 0.f : 1.f);
        pr->endChangeGesture();
    }
}

void QuadraEditor::stepValue (int dir)
{
    const auto now = juce::Time::getMillisecondCounter();
    streak = (now - lastStep < 260) ? streak + 1 : 0; lastStep = now;
    if (sel == PROG) { proc.changeProgram ((proc.getProgram() + dir + kNumPresets) % kNumPresets); return; }
    const int m = streak < 6 ? 1 : (streak < 14 ? 3 : 8);
    if (sel == STORE)
    {
        if (storePage == 0) storeDest = ((storeDest + dir * m) % 100 + 100) % 100;
        else if (storePage == 1) recallSrc = ((recallSrc + dir * m) % 100 + 100) % 100;
        return;
    }
    if (sel == NAME)   // scroll the character under the cursor through the printable set (the space is the first one)
    {
        auto nm = proc.currentName().paddedRight (' ', 14).substring (0, 14);
        int ch = (int) nm[nameCursor] + dir;
        if (ch > 126) ch = 32; if (ch < 32) ch = 126;
        if (juce::ModifierKeys::currentModifiers.isCtrlDown()) ch = 32;   // Ctrl+VALUE = space (the manual uses both VALUE buttons at once)
        proc.setCurrentName (nm.replaceSection (nameCursor, 1, juce::String::charToString ((juce::juce_wchar) ch)).trimEnd());
        edited = true;
        return;
    }
    auto pages = buildPages(); if (pages.empty()) return;
    const PDef* d = findDef (pages[(size_t) juce::jlimit (0, (int) pages.size() - 1, pageIdx)].id); if (d == nullptr) return;
    auto* pr = proc.apvts.getParameter (d->id); if (pr == nullptr) return;
    const float v = raw (d->id); float nv = v;
    if (d->kind == KC)
    {
        float hi = d->mx; const juce::String cid (d->id); const int c = (int) raw ("config");
        if (cid == "dtype" && c != 3) hi = 2.f;                       // multi tap delay is only in the 5-band configuration
        if (cid == "dsel" && c != 0 && c != 3) hi = 1.f;              // panning / tremolo only in the two configurations that have them
        nv = juce::jlimit (d->mn, hi, v + (float) dir);
    }
    else if (d->kind == KB) nv = dir > 0 ? 1.f : 0.f;
    else
    {
        const float range = d->mx - d->mn;
        const float mult = streak < 6 ? 1.f : (streak < 14 ? 4.f : (range > 1000.f ? 40.f : 12.f));
        nv = v + (float) dir * d->step * mult;
        float hi = d->mx;
        const juce::String id (d->id);
        const int cfg = (int) raw ("config");
        if (id == "dtl" || id == "dtr")   // delay time limits depend on type and configuration (manual, section 4; addendum for the resonator configuration)
        {
            const bool big = (cfg == 2 || cfg == 3);
            hi = (int) raw ("dtype") == 2 ? (big ? 1500.f : (cfg == 6 ? 720.f : 800.f)) : (big ? 750.f : (cfg == 6 ? 320.f : 400.f));
        }
        if (id.length() == 3 && id[0] == 't' && juce::CharacterFunctions::isDigit (id[1]) && id[2] == 't')   // multi tap: the 8 taps together may not exceed 1500 ms
        {
            float others = 0.f; for (int k = 0; k < 8; ++k) if (juce::String (kTapIds[k][0]) != id) others += raw (kTapIds[k][0]);
            hi = juce::jmax (0.f, 1500.f - others);
        }
        nv = juce::jlimit (d->mn, hi, nv);
        nv = d->mn + std::round ((nv - d->mn) / d->step) * d->step;
    }
    edited = true;
    pr->beginChangeGesture(); pr->setValueNotifyingHost (pr->convertTo0to1 (nv)); pr->endChangeGesture();
    proc.sysexMirror (d->id);
}

bool QuadraEditor::keyPressed (const juce::KeyPress& k)
{
    if (sel != NAME) return false;
    auto nm = proc.currentName().paddedRight (' ', 14).substring (0, 14);
    const auto step = [this] (int d) { nameCursor = (nameCursor + d + 14) % 14; repaint(); return true; };
    if (k == juce::KeyPress::leftKey)  return step (-1);
    if (k == juce::KeyPress::rightKey) return step (+1);
    if (k == juce::KeyPress::backspaceKey)
    {
        nameCursor = (nameCursor + 13) % 14;
        proc.setCurrentName (nm.replaceSection (nameCursor, 1, " ").trimEnd()); edited = true; repaint(); return true;
    }
    const auto c = k.getTextCharacter();
    if (c >= 32 && c <= 126)
    {
        proc.setCurrentName (nm.replaceSection (nameCursor, 1, juce::String::charToString (c)).trimEnd());
        edited = true; return step (+1);
    }
    return false;
}

void QuadraEditor::executeStore()
{
    const auto two = [] (int v) { return juce::String (v).paddedLeft ('0', 2); };
    if (storePage == 0)      { proc.storeProgram (storeDest);  msg = "STORED TO " + two (storeDest); }
    else if (storePage == 1) { proc.recallFactory (recallSrc); msg = "RECALLED " + two (recallSrc); }
    else                     { proc.recallAllFactory();        msg = "ALL RECALLED"; }
    msgUntil = juce::Time::getMillisecondCounter() + 1500;
    edited = false;
    sel = PROG;
}

void QuadraEditor::pressed (int z)
{
    switch (z)
    {
        case DELAY:
        {
            const bool multiTap = (int) raw ("config") == 3 && (int) raw ("dtype") == 3;
            if (sel == DELAY && multiTap && pageIdx >= 3 && pageIdx <= 7)   // addendum: DELAY steps the tap number while the tap pages are showing
            {
                if (auto* pr = proc.apvts.getParameter ("hctap"))
                { const int t = (int) raw ("hctap"); pr->setValueNotifyingHost (pr->convertTo0to1 ((float) (t >= 8 ? 1 : t + 1))); }
            }
            else if (sel != z) { sel = z; pageIdx = 0; }
            break;
        }
        case EQ:
            if ((int) raw ("config") == 7) proc.requestPlay();   // sampling configuration: EQ plays the sample
            if (sel != z) { sel = z; pageIdx = 0; }
            break;
        case PROG: case REVERB: case PITCH: case MIDI: case CONFIG: case MIX: case MOD:
            if (sel != z) { sel = z; pageIdx = 0; }
            break;
        case STORE:   // first press arms STORE, the second press carries out the displayed instruction
            if (sel == STORE) executeStore();
            else { sel = STORE; storePage = 0; storeDest = recallSrc = proc.getProgram(); }
            break;
        case NAME: if (sel != NAME) { sel = NAME; nameCursor = 0; } grabKeyboardFocus(); break;
        case BYPASS: if ((int) raw ("config") == 7) proc.requestRecord(); else toggleParam ("bypass"); break;   // sampling configuration: BYPASS records
        case PAGEUP: case PAGEDN:
        {
            if (sel == STORE) { storePage = (storePage + (z == PAGEUP ? 1 : 2)) % 3; break; }
            if (sel == NAME)  { nameCursor = (nameCursor + (z == PAGEUP ? 1 : 13)) % 14; break; }
            const int n = (int) buildPages().size();
            if (n > 0) pageIdx = (pageIdx + (z == PAGEUP ? 1 : n - 1)) % n;
            break;
        }
        case VALUP: stepValue (+1); break;
        case VALDN: stepValue (-1); break;
        default: break;
    }
    repaint();
}

juce::String QuadraEditor::valueText (const PDef& d, float v) const
{
    const juce::String id (d.id); const int cfg = (int) raw ("config");
    if (id.length() == 3 && id[0] == 'm' && juce::CharacterFunctions::isDigit (id[1]))
    {
        const int iv = (int) std::lround (v);
        if (id[2] == 's')
        {
            if (iv == 0) return "PITCH BEND"; if (iv == 1) return "AFTER TOUCH"; if (iv == 2) return "NOTE NUMBER"; if (iv == 3) return "NOTE VELOCITY";
            if (iv <= 131) return "CONTROLLER #" + juce::String (iv - 4).paddedLeft ('0', 3);
            return "HOST CONTROL " + juce::String (iv - 131);
        }
        if (id[2] == 't') return kModTargets[juce::jlimit (0, kNumModTargets - 1, iv)].label;
        return juce::String::formatted ("%+d", iv);
    }
    if (id.length() == 3 && id[0] == 't' && juce::CharacterFunctions::isDigit (id[1]))   // multi tap: time / volume / pan / feedback of one tap
    {
        const int iv = (int) std::lround (v);
        if (id[2] == 't') return juce::String::formatted ("%04d millisecs", iv);
        if (id[2] == 'v') return juce::String (iv);
        if (id[2] == 'f') return juce::String::formatted ("%02d%%", iv);
        const juce::String num = juce::String (std::abs (iv)).paddedLeft ('0', 2);   // panning: LEFT<-00->RIGHT
        return iv < 0 ? "LEFT<-" + num + "  RIGHT" : (iv > 0 ? "LEFT  " + num + "->RIGHT" : juce::String ("LEFT<-00->RIGHT"));
    }
    if (id == "mtmaster") return juce::String::formatted ("%02d%%", (int) std::lround (v));
    if (id == "rmmix" || id == "rmdr")   // ring modulator: DOWN<-00->UP
    {
        const int iv = (int) std::lround (v); const juce::String num = juce::String (std::abs (iv)).paddedLeft ('0', 2);
        return iv < 0 ? "DOWN<-" + num + "   UP" : (iv > 0 ? "DOWN   " + num + "->UP" : juce::String ("DOWN<-00->UP"));
    }
    if (id.length() == 4 && id.startsWith ("res") && juce::CharacterFunctions::isDigit (id[3])) return juce::String::formatted ("%+03d SEMITONES", (int) std::lround (v));
    if (id == "sstart" || id == "slen") return juce::String (v / 100.f, 2) + " SECONDS";
    if (id == "sauto") return juce::String ("SAMPLING: ") + (v > 0.5f ? "ON" : "OFF");
    if (id == "slow")  return juce::String::formatted ("LOW LIMIT: %03d", (int) std::lround (v));
    if (id == "sbase") return juce::String::formatted ("BASE: %03d", (int) std::lround (v));
    if (id == "shigh") return juce::String::formatted ("HIGH LIMIT: %03d", (int) std::lround (v));
    if (cfg == 7 && (id == "dlevel" || id == "slevel")) return juce::String::formatted ("LEVEL: +%02d", (int) std::lround (v));
    if (id == "midich") return v < 0.5f ? "OMNI" : "CHANNEL " + juce::String ((int) std::lround (v));
    if (d.kind == KC)
    {
        auto sa = labelsOf (d); juce::String s = sa[juce::jlimit (0, sa.size() - 1, (int) std::lround (v))];
        if (id == "rtype" && cfg == 4) s = s.dropLastCharacters (1) + "2";
        if (id == "rin1" && (int) std::lround (v) == 4) s = cfg == 5 ? "RING MOD OUTPUT" : (cfg == 6 ? "RESONATOR OUT" : s);   // EQ>REVERB uses the enhanced "2" reverbs
        return s;
    }
    if (d.kind == KB) return v > 0.5f ? "ON" : "OFF";
    if (id == "rinmix" || id == "dinmix")
    {
        const int n = std::abs ((int) std::lround (v)); const juce::String r = id == "rinmix" ? "2" : (cfg == 1 ? "LZ" : (cfg == 5 ? "RING" : (cfg == 6 ? "RES" : "PCH")));
        const juce::String num = juce::String (n).paddedLeft ('0', 2);
        return v < 0 ? "1<" + num + "  " + r : (v > 0 ? "1  " + num + ">" + r : "1<00>" + r);
    }
    if (id == "pspeed" && raw ("psync") > 0.5f) return "SYNCED TO TEMPO";
    if ((id == "dtl" && raw ("dsync") > 0.5f) || (id == "dtr" && raw ("dsyncr") > 0.5f))
    {
        const PDef* sd = findDef (id == "dtl" ? "dsync" : "dsyncr");
        return "SYNC " + labelsOf (*sd)[(int) raw (sd->id)];
    }
    if (id == "rpremix") return "PRE " + juce::String ((int) std::lround (v)) + "> POST";
    if (id == "pdet") return juce::String::formatted ("%+03d", (int) std::lround (v));
    if (id == "lzhigh") return juce::String::formatted ("%+d dB", (int) std::lround (v));
    if (d.kind == KF)
    {
        if (juce::String (d.unit) == "OCT") return juce::String (v, 2) + " OCT";
        return juce::String::formatted ("%+.1f", v) + d.unit;
    }
    return juce::String ((int) std::lround (v)) + (juce::String (d.unit).isEmpty() ? "" : juce::String (" ") + d.unit);
}

void QuadraEditor::paint (juce::Graphics& g)
{
    g.setImageResamplingQuality (juce::Graphics::highResamplingQuality);
    if (face.isValid()) g.drawImage (face, getLocalBounds().toFloat());
    else g.fillAll (juce::Colours::black);
    g.addTransform (juce::AffineTransform::scale ((float) getWidth() / (float) kW));

    // ---- LCD: amber backlit window, 2 lines x 16 characters in a 5x7 dot-matrix font like the real unit
    g.setGradientFill (juce::ColourGradient (kLcdTop, 0, kLcd.getY(), kLcdBottom, 0, kLcd.getBottom(), false));
    g.fillRoundedRectangle (kLcd, 1.4f);

    juce::String l1, l2;
    const int prog = proc.getProgram();
    const auto two = [] (int n) { return juce::String (n).paddedLeft ('0', 2); };
    int cursorAt = -1;
    const int cfgNow = (int) raw ("config");
    if (cfgNow == 7 && proc.sampleState() == 1)      { l1 = "WAITING FOR"; l2 = "AUDIO THRESHOLD"; }
    else if (cfgNow == 7 && proc.sampleState() == 2) { l1 = "SAMPLING........"; l2 = ""; }
    else if (cfgNow == 7 && sel == EQ)               // front-panel playback, as in the addendum
    {
        l1 = ((int) raw ("smode") == 0 && proc.samplePlaying()) ? "PUSH EQ TO STOP" : "PUSH EQ TO TRIG."; l2 = "SAMPLE PLAYBACK";
    }
    else if (msg.isNotEmpty() && juce::Time::getMillisecondCounter() < msgUntil) { l1 = msg; l2 = proc.currentName(); }
    else if (sel == PROG)
    {
        l1 = "PROGRAM " + two (prog) + (edited ? "." : "");
        const juce::String nm = proc.currentName();
        l2 = "\"" + juce::String (nm.length() < 14 ? " " : "") + nm + "\"";   // PROGRAM 59. / " GREASE GUITAR"
    }
    else if (sel == STORE)
    {
        if (storePage == 0)      { l1 = "STORE PROGRAM IN"; l2 = "LOCATION: " + two (storeDest) + (proc.slotIsUser (storeDest) ? " *" : ""); }
        else if (storePage == 1) { l1 = "RECALL FACTORY:"; l2 = two (recallSrc) + " " + juce::String (kPresets[recallSrc].name); }
        else                     { l1 = "RECALL ALL 90"; l2 = "ALESIS PROGRAMS"; }
    }
    else if (sel == NAME)
    {
        l1 = "EDIT NAME:"; l2 = proc.currentName().paddedRight (' ', 14).substring (0, 14); cursorAt = nameCursor;
    }
    else
    {
        auto pages = buildPages();
        if (pages.empty()) { l1 = kZones[sel].name; l2 = "NOT IN CONFIG"; }
        else
        {
            pageIdx = juce::jlimit (0, (int) pages.size() - 1, pageIdx);
            const PDef* d = findDef (pages[(size_t) pageIdx].id);
            l1 = pages[(size_t) pageIdx].title; if (d != nullptr) l2 = valueText (*d, raw (d->id));
        }
    }
    drawDotLine (g, l1.substring (0, 16), kLcdTextX, kLcdLine1Y, -1);
    drawDotLine (g, l2.substring (0, 16), kLcdTextX, kLcdLine2Y, cursorAt);

    // ---- input level LEDs: CLIP, -6, -12, -18
    const float thr[4] = { 0.95f, 0.501f, 0.251f, 0.126f };
    for (int i = 0; i < 4; ++i)
    {
        const bool on = level > thr[i];
        auto c = i == 0 ? juce::Colour (0xffff3030) : juce::Colour (0xff40ff60);
        if (on) { g.setColour (c.withAlpha (0.35f)); g.fillEllipse (kLeds[i].x - 3.4f, kLeds[i].y - 3.4f, 6.8f, 6.8f); }
        g.setColour (on ? c : c.withAlpha (0.0f)); g.fillEllipse (kLeds[i].x - 2.f, kLeds[i].y - 2.f, 4.f, 4.f);
    }

    // ---- button LEDs: the selected function lights red, bypass lights amber
    auto light = [&] (int z, juce::Colour c)
    {
        const auto p = juce::Point<float> (kZones[z].cx, kZones[z].cy);
        g.setColour (c.withAlpha (0.35f)); g.fillEllipse (p.x - 3.4f, p.y - 3.4f, 6.8f, 6.8f);
        g.setColour (c); g.fillEllipse (p.x - 1.8f, p.y - 1.8f, 3.6f, 3.6f);
    };
    light (sel, juce::Colour (0xffff3b2a));
    if (raw ("bypass") > 0.5f) light (BYPASS, juce::Colour (0xffffb020));
}
