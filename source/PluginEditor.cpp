#include "PluginEditor.h"

PluginEditor::PluginEditor (PluginProcessor& p)
    : AudioProcessorEditor (&p), processorRef (p)
{
    // Set up the gain slider
    gainSlider.setSliderStyle (juce::Slider::LinearVertical);
    gainSlider.setRange (0.0, 1.0, 0.01);
    gainSlider.setTextBoxStyle (juce::Slider::TextBoxBelow, false, 90, 20);
    gainSlider.setValue (processorRef.getParameters()[0]->getValue());
    gainSlider.onValueChange = [this]
    {
        processorRef.getParameters()[0]->setValueNotifyingHost (gainSlider.getValue());
    };
    addAndMakeVisible (gainSlider);

    // Set up the label
    gainLabel.setText ("Gain", juce::dontSendNotification);
    gainLabel.attachToComponent (&gainSlider, false);
    gainLabel.setJustificationType (juce::Justification::centred);
    addAndMakeVisible (gainLabel);

    addAndMakeVisible (inspectButton);

    // this chunk of code instantiates and opens the melatonin inspector
    inspectButton.onClick = [&] {
        if (!inspector)
        {
            inspector = std::make_unique<melatonin::Inspector> (*this);
            inspector->onClose = [this]() { inspector.reset(); };
        }

        inspector->setVisible (true);
    };

    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.
    setSize (400, 300);
}

PluginEditor::~PluginEditor()
{
}

void PluginEditor::paint (juce::Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));

    g.setColour (juce::Colours::white);
    g.setFont (20.0f);
    g.drawText ("Simple Gain Plugin", getLocalBounds().removeFromTop (40), juce::Justification::centred, true);
}

void PluginEditor::resized()
{
    // layout the positions of your child components here
    auto area = getLocalBounds();
    area.removeFromTop (50);  // Space for title

    // Center the slider
    auto sliderArea = area.removeFromTop (180);
    gainSlider.setBounds (sliderArea.withSizeKeepingCentre (80, 150));

    // Position inspect button at bottom
    inspectButton.setBounds (area.withSizeKeepingCentre (100, 30));
}
