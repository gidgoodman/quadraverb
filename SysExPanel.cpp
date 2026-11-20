#include "SysExPanel.h"

SysExPanel::SysExPanel (QuadraVerbSysEx& s) : sx (s)
{
    panelLaf.setColour (juce::TextButton::buttonColourId, juce::Colour (0xff34343a));
    panelLaf.setColour (juce::TextButton::textColourOffId, juce::Colour (0xfff0f0f0));
    panelLaf.setColour (juce::ComboBox::backgroundColourId, juce::Colour (0xff1e1e22));
    panelLaf.setColour (juce::ComboBox::textColourId, juce::Colour (0xfff0f0f0));
    panelLaf.setColour (juce::ComboBox::outlineColourId, juce::Colour (0xff555560));
    panelLaf.setColour (juce::TextEditor::backgroundColourId, juce::Colour (0xff1e1e22));
    panelLaf.setColour (juce::TextEditor::textColourId, juce::Colour (0xfff0f0f0));
    panelLaf.setColour (juce::TextEditor::outlineColourId, juce::Colour (0xff555560));
    panelLaf.setColour (juce::PopupMenu::backgroundColourId, juce::Colour (0xff1e1e22));
    panelLaf.setColour (juce::PopupMenu::textColourId, juce::Colour (0xfff0f0f0));
    setLookAndFeel (&panelLaf);
    for (auto* l : { &outL, &inL, &funcL, &pageL, &valL }) { l->setFont (juce::Font (12.f, juce::Font::bold)); l->setColour (juce::Label::textColourId, juce::Colour (0xffd8d8d8)); addAndMakeVisible (*l); }
    status.setFont (juce::Font (11.f)); status.setColour (juce::Label::textColourId, juce::Colour (0xffe6a417)); addAndMakeVisible (status);
    for (auto* c : { &outBox, &inBox }) addAndMakeVisible (*c);
    for (auto* t : { &hwToggle, &plugToggle, &mirrorToggle, &captureToggle }) { t->setColour (juce::ToggleButton::textColourId, juce::Colour (0xffd8d8d8)); addAndMakeVisible (*t); }
    for (auto* b : { &sendDumpBtn, &importBtn, &exportBtn, &clearBtn, &rescanBtn, &closeBtn, &testBtn }) addAndMakeVisible (*b);
    for (auto* e : { &funcEd, &pageEd, &valEd }) { e->setMultiLine (false); e->setJustification (juce::Justification::centred); addAndMakeVisible (*e); }
    funcEd.setText ("01", false); pageEd.setText ("00", false); valEd.setText ("0", false);
    funcEd.setInputRestrictions (2, "0123456789abcdefABCDEF"); pageEd.setInputRestrictions (2, "0123456789abcdefABCDEF"); valEd.setInputRestrictions (5, "0123456789");
    funcEd.setTooltip ("function byte, hex"); pageEd.setTooltip ("page byte, hex"); valEd.setTooltip ("16-bit value, 0-65535");

    hwToggle.setToggleState (sx.usesHardwareOutput(), juce::dontSendNotification); hwToggle.onClick = [this] { sx.setUseHardwareOutput (hwToggle.getToggleState()); };
    plugToggle.setToggleState (sx.usesPluginOutput(), juce::dontSendNotification);  plugToggle.onClick = [this] { sx.setUsePluginOutput (plugToggle.getToggleState()); };
    mirrorToggle.setToggleState (sx.mirrorEnabled(), juce::dontSendNotification);   mirrorToggle.onClick = [this] { sx.setMirrorEnabled (mirrorToggle.getToggleState()); };
    mirrorToggle.setTooltip ("Needs page numbers in SysExMap.h; nothing is sent for unmapped parameters");
    captureToggle.setToggleState (sx.isCapturing(), juce::dontSendNotification);    captureToggle.onClick = [this] { sx.setCapture (captureToggle.getToggleState()); };

    outBox.onChange = [this] { const int i = outBox.getSelectedId() - 2; sx.openOutput (i >= 0 && i < outIds.size() ? outIds[i] : juce::String()); };
    inBox.onChange  = [this] { const int i = inBox.getSelectedId() - 2;  sx.openInput  (i >= 0 && i < inIds.size()  ? inIds[i]  : juce::String()); };
    rescanBtn.onClick = [this] { refreshDevices(); };
    closeBtn.onClick = [this] { if (onClose) onClose(); };
    sendDumpBtn.onClick = [this] { sx.sendDump(); };
    clearBtn.onClick = [this] { sx.clearDump(); };
    importBtn.onClick = [this] { chooseImport(); };
    exportBtn.onClick = [this] { chooseExport(); };
    testBtn.onClick = [this] { sendTest(); };
    refreshDevices(); startTimerHz (5);
}

void SysExPanel::refreshDevices()
{
    outIds.clear(); inIds.clear(); outBox.clear (juce::dontSendNotification); inBox.clear (juce::dontSendNotification);
    outBox.addItem ("(none)", 1); inBox.addItem ("(none)", 1);
    int id = 2; for (auto& d : QuadraVerbSysEx::getOutputDevices()) { outBox.addItem (d.name, id++); outIds.add (d.identifier); }
    id = 2;     for (auto& d : QuadraVerbSysEx::getInputDevices())  { inBox.addItem (d.name, id++);  inIds.add (d.identifier); }
    outBox.setSelectedId (outIds.indexOf (sx.outputId()) + 2 > 1 && sx.outputId().isNotEmpty() ? outIds.indexOf (sx.outputId()) + 2 : 1, juce::dontSendNotification);
    inBox.setSelectedId  (inIds.indexOf (sx.inputId()) + 2 > 1 && sx.inputId().isNotEmpty() ? inIds.indexOf (sx.inputId()) + 2 : 1, juce::dontSendNotification);
}

void SysExPanel::timerCallback()
{
    if (! isVisible()) return;
    status.setText ("Queue " + juce::String (sx.queueSize()) + "   Sent " + juce::String (sx.sentCount()) + "   Rx " + juce::String (sx.receivedCount())
                    + "   Dump " + juce::String (sx.dumpMessageCount()) + " msgs / " + juce::String (sx.dumpByteCount()) + " bytes"
                    + (sx.droppedCount() ? "   Dropped " + juce::String (sx.droppedCount()) : juce::String())
                    + (sx.lastRx().isNotEmpty() ? "   Last Rx: " + sx.lastRx() : juce::String()), juce::dontSendNotification);
    captureToggle.setToggleState (sx.isCapturing(), juce::dontSendNotification);
}

void SysExPanel::chooseImport()
{
    chooser = std::make_unique<juce::FileChooser> ("Import a QuadraVerb SysEx dump", juce::File(), "*.syx");
    chooser->launchAsync (juce::FileBrowserComponent::openMode | juce::FileBrowserComponent::canSelectFiles, [this] (const juce::FileChooser& fc)
    {
        const auto f = fc.getResult(); if (f == juce::File()) return;
        const int n = sx.importSyx (f);
        status.setText (n < 0 ? "Import failed: no SysEx messages in " + f.getFileName() : "Imported " + juce::String (n) + " messages from " + f.getFileName(), juce::dontSendNotification);
    });
}
void SysExPanel::chooseExport()
{
    chooser = std::make_unique<juce::FileChooser> ("Export the SysEx dump", juce::File::getSpecialLocation (juce::File::userDocumentsDirectory).getChildFile ("QuadraVerb.syx"), "*.syx");
    chooser->launchAsync (juce::FileBrowserComponent::saveMode | juce::FileBrowserComponent::canSelectFiles | juce::FileBrowserComponent::warnAboutOverwriting, [this] (const juce::FileChooser& fc)
    {
        auto f = fc.getResult(); if (f == juce::File()) return;
        if (f.getFileExtension().isEmpty()) f = f.withFileExtension (".syx");
        status.setText (sx.exportSyx (f) ? "Exported " + juce::String (sx.dumpMessageCount()) + " messages to " + f.getFileName() : "Nothing to export (capture or import a dump first)", juce::dontSendNotification);
    });
}
void SysExPanel::sendTest()
{
    const auto func = (juce::uint8) (funcEd.getText().getHexValue32() & 0x7F), page = (juce::uint8) (pageEd.getText().getHexValue32() & 0x7F);
    const auto value = (juce::uint16) juce::jlimit (0, 65535, valEd.getText().getIntValue());
    sx.enqueueParam (func, page, value);
}

void SysExPanel::paint (juce::Graphics& g)
{
    g.setColour (juce::Colour (0xff101012)); g.fillRoundedRectangle (getLocalBounds().toFloat().reduced (2.f), 4.f);
    g.setColour (juce::Colour (0xff3a3a3d)); g.drawRoundedRectangle (getLocalBounds().toFloat().reduced (2.f), 4.f, 1.f);
    g.setColour (juce::Colour (0xff2b8cff)); g.setFont (juce::Font (13.f, juce::Font::bold | juce::Font::italic));
    g.drawText ("QuadraVerb SysEx / MIDI", getLocalBounds().reduced (10, 5).removeFromTop (juce::jmax (16, getHeight() / 6)), juce::Justification::centredLeft);
}

void SysExPanel::resized()
{
    auto r = getLocalBounds().reduced (10, 5);
    const int rh = juce::jlimit (16, 26, r.getHeight() / 5), gap = 2;
    auto row = [&] { auto x = r.removeFromTop (rh); r.removeFromTop (gap); return x; };
    { auto t = row(); closeBtn.setBounds (t.removeFromRight (rh + 6)); }                                       // title row (title is painted)
    { auto t = row(); outL.setBounds (t.removeFromLeft (70)); outBox.setBounds (t.removeFromLeft (juce::jmax (140, t.getWidth() / 3)));
      t.removeFromLeft (12); inL.setBounds (t.removeFromLeft (60)); inBox.setBounds (t.removeFromLeft (juce::jmax (140, t.getWidth() / 2))); t.removeFromLeft (8); rescanBtn.setBounds (t.removeFromLeft (70)); }
    { auto t = row(); const int w = juce::jmax (110, t.getWidth() / 5); hwToggle.setBounds (t.removeFromLeft (w)); plugToggle.setBounds (t.removeFromLeft (w)); mirrorToggle.setBounds (t.removeFromLeft (w + 20)); captureToggle.setBounds (t.removeFromLeft (w)); }
    { auto t = row(); const int w = 96; sendDumpBtn.setBounds (t.removeFromLeft (w)); t.removeFromLeft (4); importBtn.setBounds (t.removeFromLeft (w)); t.removeFromLeft (4);
      exportBtn.setBounds (t.removeFromLeft (w)); t.removeFromLeft (4); clearBtn.setBounds (t.removeFromLeft (60)); t.removeFromLeft (10); status.setBounds (t); }
    { auto t = row(); funcL.setBounds (t.removeFromLeft (44)); funcEd.setBounds (t.removeFromLeft (40)); t.removeFromLeft (8); pageL.setBounds (t.removeFromLeft (44)); pageEd.setBounds (t.removeFromLeft (40)); t.removeFromLeft (8);
      valL.setBounds (t.removeFromLeft (50)); valEd.setBounds (t.removeFromLeft (64)); t.removeFromLeft (8); testBtn.setBounds (t.removeFromLeft (60)); }
}
