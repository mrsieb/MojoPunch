#include "PluginEditor.h"

void PluginEditor::configureRotary (juce::Slider& slider, juce::Label& label, const juce::String& text)
{
    slider.setSliderStyle (juce::Slider::RotaryHorizontalVerticalDrag);
    slider.setTextBoxStyle (juce::Slider::TextBoxBelow, false, 70, 18);
    addAndMakeVisible (slider);

    label.setText (text, juce::dontSendNotification);
    label.setJustificationType (juce::Justification::centred);
    label.attachToComponent (&slider, false);
    addAndMakeVisible (label);
}

//==============================================================================
PluginEditor::PluginEditor (PluginProcessor& p)
    : AudioProcessorEditor (&p), processorRef (p)
{
    auto& apvts = processorRef.apvts;

    configureRotary (lowFreqSlider,    lowFreqLabel,    "Freq");
    configureRotary (lowGainSlider,    lowGainLabel,    "Gain");
    configureRotary (lowQSlider,       lowQLabel,       "Q");
    configureRotary (midFreqSlider,    midFreqLabel,    "Freq");
    configureRotary (midGainSlider,    midGainLabel,    "Gain");
    configureRotary (midQSlider,       midQLabel,       "Q");
    configureRotary (highFreqSlider,   highFreqLabel,   "Freq");
    configureRotary (highGainSlider,   highGainLabel,   "Gain");
    configureRotary (highQSlider,      highQLabel,      "Q");
    configureRotary (masterGainSlider, masterGainLabel, "Master");

    lowFreqAttach    = std::make_unique<SliderAttachment> (apvts, "lowFreq",    lowFreqSlider);
    lowGainAttach    = std::make_unique<SliderAttachment> (apvts, "lowGain",    lowGainSlider);
    lowQAttach       = std::make_unique<SliderAttachment> (apvts, "lowQ",       lowQSlider);
    midFreqAttach    = std::make_unique<SliderAttachment> (apvts, "midFreq",    midFreqSlider);
    midGainAttach    = std::make_unique<SliderAttachment> (apvts, "midGain",    midGainSlider);
    midQAttach       = std::make_unique<SliderAttachment> (apvts, "midQ",       midQSlider);
    highFreqAttach   = std::make_unique<SliderAttachment> (apvts, "highFreq",   highFreqSlider);
    highGainAttach   = std::make_unique<SliderAttachment> (apvts, "highGain",   highGainSlider);
    highQAttach      = std::make_unique<SliderAttachment> (apvts, "highQ",      highQSlider);
    masterGainAttach = std::make_unique<SliderAttachment> (apvts, "masterGain", masterGainSlider);

    addAndMakeVisible (inspectButton);
    inspectButton.onClick = [&] {
        if (!inspector)
        {
            inspector = std::make_unique<melatonin::Inspector> (*this);
            inspector->onClose = [this]() { inspector.reset(); };
        }
        inspector->setVisible (true);
    };

    setSize (720, 380);
}

PluginEditor::~PluginEditor()
{
}

void PluginEditor::paint (juce::Graphics& g)
{
    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));

    g.setColour (juce::Colours::white);
    g.setFont (juce::FontOptions (14.0f).withStyle ("Bold"));
    g.drawText ("MojoPunch EQ", 0, 0, getWidth(), 24, juce::Justification::centred, true);

    g.setFont (18.0f);

    // Band labels at the top of each EQ column
    const int colW = 160;
    const int startX = 40;
    g.drawText ("Low Shelf",  startX,            8, colW, 24, juce::Justification::centred, true);
    g.drawText ("Mid Bell",   startX + colW,     8, colW, 24, juce::Justification::centred, true);
    g.drawText ("High Shelf", startX + colW * 2, 8, colW, 24, juce::Justification::centred, true);
    g.drawText ("Master",     startX + colW * 3, 8,  80, 24, juce::Justification::centred, true);
}

void PluginEditor::resized()
{
    // Layout: 3 EQ columns (160 px each) + 1 master column (80 px)
    // Knobs: 80 x 80 px, centred in their column cell
    // Labels attached above via attachToComponent — give 24 px headroom per row

    const int topMargin   = 40;   // space for band title + label
    const int labelHeight = 24;
    const int knobSize    = 80;
    const int rowHeight   = labelHeight + knobSize;
    const int colW        = 160;
    const int startX      = 40;

    auto placeKnob = [&] (juce::Slider& s, int col, int row)
    {
        const int x = startX + col * colW + (colW - knobSize) / 2;
        const int y = topMargin + row * rowHeight + labelHeight;
        s.setBounds (x, y, knobSize, knobSize);
    };

    // Low Shelf column (col 0)
    placeKnob (lowFreqSlider,  0, 0);
    placeKnob (lowGainSlider,  0, 1);
    placeKnob (lowQSlider,     0, 2);

    // Mid Bell column (col 1)
    placeKnob (midFreqSlider,  1, 0);
    placeKnob (midGainSlider,  1, 1);
    placeKnob (midQSlider,     1, 2);

    // High Shelf column (col 2)
    placeKnob (highFreqSlider, 2, 0);
    placeKnob (highGainSlider, 2, 1);
    placeKnob (highQSlider,    2, 2);

    // Master column (col 3, narrower — centre in 80 px)
    const int masterX = startX + 3 * colW + (80 - knobSize) / 2;
    const int masterY = topMargin + labelHeight;
    masterGainSlider.setBounds (masterX, masterY, knobSize, knobSize);

    // Inspect button at the bottom
    inspectButton.setBounds (getWidth() / 2 - 50, getHeight() - 34, 100, 28);
}
