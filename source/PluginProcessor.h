#pragma once

#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_dsp/juce_dsp.h>

#if (MSVC)
#include "ipps.h"
#endif

class PluginProcessor : public juce::AudioProcessor
{
public:
    // Type aliases
    using MonoFilter = juce::dsp::IIR::Filter<float>;
    using MonoChain  = juce::dsp::ProcessorChain<MonoFilter, MonoFilter, MonoFilter>;

    enum ChainIndex { LowShelf = 0, PeakBell = 1, HighShelf = 2 };

    PluginProcessor();
    ~PluginProcessor() override;

    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;

    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    const juce::String getName() const override;

    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;

    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram (int index) override;
    const juce::String getProgramName (int index) override;
    void changeProgramName (int index, const juce::String& newName) override;

    void getStateInformation (juce::MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;

    // Public so the editor can attach sliders
    juce::AudioProcessorValueTreeState apvts;

private:
    MonoChain leftChain, rightChain;
    double currentSampleRate { 44100.0 };

    // Cached atomic param pointers (populated in constructor body)
    std::atomic<float>* rawLowFreq   {};
    std::atomic<float>* rawLowGain   {};
    std::atomic<float>* rawLowQ      {};
    std::atomic<float>* rawMidFreq   {};
    std::atomic<float>* rawMidGain   {};
    std::atomic<float>* rawMidQ      {};
    std::atomic<float>* rawHighFreq  {};
    std::atomic<float>* rawHighGain  {};
    std::atomic<float>* rawHighQ     {};
    std::atomic<float>* rawMasterGain {};

    // Snapshot of EQ params; used to skip coefficient recalc when nothing changed.
    struct FilterParams {
        float lowFreq{}, lowGain{}, lowQ{};
        float midFreq{}, midGain{}, midQ{};
        float highFreq{}, highGain{}, highQ{};
        bool operator==(const FilterParams&) const = default;
    };
    FilterParams lastParams;

    // Smoothed master gain — eliminates clicks when the master knob moves fast.
    juce::SmoothedValue<float, juce::ValueSmoothingTypes::Linear> smoothedGain;

    FilterParams readParams() const noexcept;
    void updateFilters (const FilterParams& p);
    static void writeCoeffsInPlace (juce::dsp::IIR::Coefficients<float>&,
                                    float b0, float b1, float b2,
                                    float a0, float a1, float a2) noexcept;
    static juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout();

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (PluginProcessor)
};
