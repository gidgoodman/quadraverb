#include "PluginProcessor.h"
#include <cmath>

namespace
{
    // Classic 5x7 dot-matrix LCD font (ASCII 32-95)
    static const unsigned char kFont5x7[64 * 5] = {
        0x00,0x00,0x00,0x00,0x00, 0x00,0x00,0x5F,0x00,0x00, 0x00,0x07,0x00,0x07,0x00, 0x14,0x7F,0x14,0x7F,0x14,
        0x24,0x2A,0x7F,0x2A,0x12, 0x23,0x13,0x08,0x64,0x62, 0x36,0x49,0x56,0x20,0x50, 0x00,0x05,0x03,0x00,0x00,
        0x00,0x1C,0x22,0x41,0x00, 0x00,0x41,0x22,0x1C,0x00, 0x14,0x08,0x3E,0x08,0x14, 0x08,0x08,0x3E,0x08,0x08,
        0x00,0x50,0x30,0x00,0x00, 0x08,0x08,0x08,0x08,0x08, 0x00,0x60,0x60,0x00,0x00, 0x20,0x10,0x08,0x04,0x02,
        0x3E,0x51,0x49,0x45,0x3E, 0x00,0x42,0x7F,0x40,0x00, 0x42,0x61,0x51,0x49,0x46, 0x21,0x41,0x45,0x4B,0x31,
        0x18,0x14,0x12,0x7F,0x10, 0x27,0x45,0x45,0x45,0x39, 0x3C,0x4A,0x49,0x49,0x30, 0x01,0x71,0x09,0x05,0x03,
        0x36,0x49,0x49,0x49,0x36, 0x06,0x49,0x49,0x29,0x1E, 0x00,0x36,0x36,0x00,0x00, 0x00,0x56,0x36,0x00,0x00,
        0x08,0x14,0x22,0x41,0x00, 0x14,0x14,0x14,0x14,0x14, 0x00,0x41,0x22,0x14,0x08, 0x02,0x01,0x51,0x09,0x06,
        0x32,0x49,0x79,0x41,0x3E, 0x7E,0x11,0x11,0x11,0x7E, 0x7F,0x49,0x49,0x49,0x36, 0x3E,0x41,0x41,0x41,0x22,
        0x7F,0x41,0x41,0x22,0x1C, 0x7F,0x49,0x49,0x49,0x41, 0x7F,0x09,0x09,0x09,0x01, 0x3E,0x41,0x49,0x49,0x7A,
        0x7F,0x08,0x08,0x08,0x7F, 0x00,0x41,0x7F,0x41,0x00, 0x20,0x40,0x41,0x3F,0x01, 0x7F,0x08,0x14,0x22,0x41,
        0x7F,0x40,0x40,0x40,0x40, 0x7F,0x02,0x0C,0x02,0x7F, 0x7F,0x04,0x08,0x10,0x7F, 0x3E,0x41,0x41,0x41,0x3E,
        0x7F,0x09,0x09,0x09,0x06, 0x3E,0x41,0x51,0x21,0x5E, 0x7F,0x09,0x19,0x29,0x46, 0x46,0x49,0x49,0x49,0x31,
        0x01,0x01,0x7F,0x01,0x01, 0x3F,0x40,0x40,0x40,0x3F, 0x1F,0x20,0x40,0x20,0x1F, 0x3F,0x40,0x38,0x40,0x3F,
        0x63,0x14,0x08,0x14,0x63, 0x07,0x08,0x70,0x08,0x07, 0x61,0x51,0x49,0x45,0x43, 0x00,0x7F,0x41,0x41,0x00,
        0x02,0x04,0x08,0x10,0x20, 0x00,0x41,0x41,0x7F,0x00, 0x04,0x02,0x01,0x02,0x04, 0x40,0x40,0x40,0x40,0x40
    };

    void drawDotLine (juce::Graphics& g, const juce::String& text, float x0, float y0, float dot)
    {
        const juce::String up = text.toUpperCase();
        juce::Path lit, ghost;
        for (int ci = 0; ci < 16; ++ci)
        {
            const int gi = juce::jlimit (0, 63, (ci < up.length() ? (int) up[ci] : 32) - 32);
            for (int col = 0; col < 5; ++col)
                for (int row = 0; row < 7; ++row)
                {
                    const juce::Rectangle<float> r (x0 + (ci * 6 + col) * dot, y0 + row * dot, dot * 0.85f, dot * 0.85f);
                    if (((kFont5x7[gi * 5 + col] >> row) & 1) != 0) lit.addRectangle (r);
                    else ghost.addRectangle (r);
                }
        }
        g.setColour (juce::Colour (0xff3a2a0c).withAlpha (0.08f));
        g.fillPath (ghost);
        g.setColour (juce::Colour (0xff2b1e06));
        g.fillPath (lit);
    }

    int readAlesisBits (const juce::uint8* buf, int bitOffset, int numBits, bool isSigned = false)
    {
        int val = 0;
        for (int i = 0; i < numBits; ++i)
        {
            const int bitPos = bitOffset + i;
            const int byteIdx = bitPos / 7;
            const int bitInByte = 6 - (bitPos % 7);
            if (byteIdx < 147) val = (val << 1) | ((buf[byteIdx] >> bitInByte) & 1);
        }
        if (isSigned && (val & (1 << (numBits - 1)))) val -= (1 << numBits);
        return val;
    }

    void writeAlesisBits (juce::uint8* buf, int bitOffset, int numBits, int val)
    {
        for (int i = 0; i < numBits; ++i)
        {
            const int bitPos = bitOffset + i;
            const int byteIdx = bitPos / 7;
            const int bitInByte = 6 - (bitPos % 7);
            if (byteIdx < 147)
            {
                if ((val >> (numBits - 1 - i)) & 1) buf[byteIdx] |= (1 << bitInByte);
                else buf[byteIdx] &= ~(1 << bitInByte);
            }
        }
    }

    float freqToX (float f, float w) { return juce::jlimit (0.f, 1.f, (std::log10 (juce::jmax (16.f, f)) - 1.20412f) / 3.09691f) * w; }
    float xToFreq (float x, float w) { return std::pow (10.f, 1.20412f + juce::jlimit (0.f, 1.f, x / juce::jmax (1.f, w)) * 3.09691f); }
    float dbToY (float db, float h)  { return h * (0.5f - db / 32.f); }
    float yToDb (float y, float h)   { return juce::jlimit (-14.f, 14.f, (0.5f - y / juce::jmax (1.f, h)) * 32.f); }
}

// ==============================================================================
// Internal QVERB Visual Editor + Interactive EQ & Multi-Tap Delay Visualizers
// ==============================================================================
class QVerbMatrixPanel : public juce::Component
{
public:
    explicit QVerbMatrixPanel (QuadraProcessor& p) : proc (p)
    {
        laf.setColour (juce::TextButton::buttonColourId, juce::Colour (0xff26262c));
        laf.setColour (juce::TextButton::buttonOnColourId, juce::Colour (0xffe6a417));
        laf.setColour (juce::TextButton::textColourOffId, juce::Colours::white);
        laf.setColour (juce::TextButton::textColourOnId, juce::Colours::black);
        laf.setColour (juce::Slider::thumbColourId, juce::Colour (0xffe6a417));
        laf.setColour (juce::Slider::trackColourId, juce::Colour (0xff484852));
        laf.setColour (juce::Slider::textBoxTextColourId, juce::Colour (0xfff5c242));
        laf.setColour (juce::Slider::textBoxOutlineColourId, juce::Colour (0xff383840));
        setLookAndFeel (&laf);

        addAndMakeVisible (importBtn);
        addAndMakeVisible (exportBtn);
        addAndMakeVisible (bankBox);
        addAndMakeVisible (statusLbl);
        addAndMakeVisible (visArea);

        statusLbl.setColour (juce::Label::textColourId, juce::Colour (0xffe6a417));
        statusLbl.setFont (juce::Font (12.f, juce::Font::bold));
        statusLbl.setText ("QVERB VISUAL EDITOR — CLICK A BLOCK OR IMPORT HARDWARE .SYX", juce::dontSendNotification);

        for (int i = 0; i < kNumPresets; ++i)
            bankBox.addItem (juce::String (i).paddedLeft ('0', 2) + ": " + proc.slotName (i), i + 1);
        bankBox.setSelectedId (proc.getProgram() + 1, juce::dontSendNotification);
        bankBox.onChange = [this]
        {
            const int idx = bankBox.getSelectedId() - 1;
            if (idx >= 0 && idx < kNumPresets) { proc.changeProgram (idx); rebuildControls(); }
        };

        importBtn.onClick = [this] { importSyx(); };
        exportBtn.onClick = [this] { exportSyx(); };

        const char* names[6] = { "ROUTING & MIX", "EQ / GRAPHIC", "PITCH / MOD", "DELAY / TAPS", "REVERB", "ALL PARAMS" };
        for (int i = 0; i < 6; ++i)
        {
            tabs[(size_t) i].setButtonText (names[i]);
            tabs[(size_t) i].onClick = [this, i] { setTab (i); };
            addAndMakeVisible (tabs[(size_t) i]);
        }

        visArea.onDrag = [this] (const char* id, float val)
        {
            if (auto* pr = proc.apvts.getParameter (id))
            {
                const auto* def = findParamDef (id);
                pr->setValueNotifyingHost (pr->convertTo0to1 (def ? juce::jlimit (def->mn, def->mx, val) : val));
                highlightedId = id;
                if (onTouch) onTouch (tabToZone[activeTab], id);
                repaint();
            }
        };

        viewport.setViewedComponent (&container, false);
        addAndMakeVisible (viewport);
        setTab (0);
    }

    ~QVerbMatrixPanel() override { setLookAndFeel (nullptr); }

    std::function<void (int zone, const char* paramId)> onTouch;
    int activeTab = 0;
    juce::String highlightedId;
    static constexpr int tabToZone[6] = { 7, 4, 3, 2, 1, 0 };

    void setTab (int t)
    {
        activeTab = juce::jlimit (0, 5, t);
        for (int i = 0; i < 6; ++i) tabs[(size_t) i].setToggleState (i == activeTab, juce::dontSendNotification);
        visArea.mode = (activeTab == 1) ? 1 : ((activeTab == 3) ? 2 : 0);
        visArea.setVisible (visArea.mode != 0);
        rebuildControls();
        resized();
        repaint();
    }

    void syncFromHardware (int zone, const char* paramId)
    {
        if (paramId != nullptr) highlightedId = paramId;
        int t = activeTab;
        if (zone == 4) t = 1;
        else if (zone == 3) t = 2;
        else if (zone == 2) t = 3;
        else if (zone == 1) t = 4;
        else if (zone == 6 || zone == 7) t = 0;
        if (t != activeTab) setTab (t);
        else { visArea.repaint(); repaint(); }
    }

    void rebuildControls()
    {
        controls.clear();
        container.removeAllChildren();
        const int cfg = (int) proc.apvts.getRawParameterValue ("config")->load();
        std::vector<const char*> ids;
        auto add = [&] (const char* id) { ids.push_back (id); };

        if (activeTab == 0) { add ("config"); add ("dlevel"); add ("master"); add ("eqlevel"); add ("plevel"); add ("dylevel"); add ("rlevel"); }
        else if (activeTab == 1)
        {
            if (cfg == 2) { const char* g = {"g0","g1","g2","g3","g4","g5","g6","g7","g8","g9","g10"}; for (auto* s : g) add (s); }
            else { add ("elf"); add ("ela"); add ("emf"); add ("embw"); add ("ema"); add ("ehf"); add ("eha"); if (cfg == 3) { add ("elmf"); add ("elma"); add ("ehmf"); add ("ehma"); } }
        }
        else if (activeTab == 2) { add ("pmode"); add ("pspeed"); add ("pdepth"); add ("pfb"); add ("pdet"); add ("rmshift"); }
        else if (activeTab == 3)
        {
            add ("dtype"); add ("dtl"); add ("dfl"); add ("dtr"); add ("dfr");
