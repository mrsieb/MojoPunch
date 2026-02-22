#pragma once

#include "PluginProcessor.h"
#include "BinaryData.h"
#include "melatonin_inspector/melatonin_inspector.h"

//==============================================================================
class PluginEditor : public juce::AudioProcessorEditor
{
public:
    explicit PluginEditor (PluginProcessor&);
    ~PluginEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;

private:
    using SliderAttachment = juce::AudioProcessorValueTreeState::SliderAttachment;

    PluginProcessor& processorRef;
    std::unique_ptr<melatonin::Inspector> inspector;
    juce::TextButton inspectButton { "Inspect the UI" };

    // Sliders — declared before attachments (attachments must be destroyed first)
    juce::Slider lowFreqSlider,  lowGainSlider,  lowQSlider;
    juce::Slider midFreqSlider,  midGainSlider,  midQSlider;
    juce::Slider highFreqSlider, highGainSlider, highQSlider;
    juce::Slider masterGainSlider;

    juce::Label lowFreqLabel,  lowGainLabel,  lowQLabel;
    juce::Label midFreqLabel,  midGainLabel,  midQLabel;
    juce::Label highFreqLabel, highGainLabel, highQLabel;
    juce::Label masterGainLabel;

    // Attachments — declared after sliders, destroyed before sliders
    std::unique_ptr<SliderAttachment> lowFreqAttach,  lowGainAttach,  lowQAttach;
    std::unique_ptr<SliderAttachment> midFreqAttach,  midGainAttach,  midQAttach;
    std::unique_ptr<SliderAttachment> highFreqAttach, highGainAttach, highQAttach;
    std::unique_ptr<SliderAttachment> masterGainAttach;

    void configureRotary (juce::Slider&, juce::Label&, const juce::String&);

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (PluginEditor)
};
