#include "PluginProcessor.h"
#include "PluginEditor.h"

using namespace juce;

//==============================================================================
juce::AudioProcessorValueTreeState::ParameterLayout PluginProcessor::createParameterLayout()
{
    std::vector<std::unique_ptr<juce::RangedAudioParameter>> params;

    params.push_back (std::make_unique<juce::AudioParameterFloat> (
        "masterGain", "Master Gain",
        juce::NormalisableRange<float> (0.0f, 1.0f, 0.001f),
        0.5f));

    // Low Shelf
    {
        juce::NormalisableRange<float> freqRange (20.0f, 800.0f, 0.1f);
        freqRange.setSkewForCentre (200.0f);
        params.push_back (std::make_unique<juce::AudioParameterFloat> ("lowFreq", "Low Freq", freqRange, 200.0f));
    }
    params.push_back (std::make_unique<juce::AudioParameterFloat> (
        "lowGain", "Low Gain",
        juce::NormalisableRange<float> (-12.0f, 12.0f, 0.1f),
        0.0f));
    {
        juce::NormalisableRange<float> qRange (0.1f, 10.0f, 0.01f);
        qRange.setSkewForCentre (1.0f);
        params.push_back (std::make_unique<juce::AudioParameterFloat> ("lowQ", "Low Q", qRange, 0.707f));
    }

    // Mid Peak/Bell
    {
        juce::NormalisableRange<float> freqRange (200.0f, 8000.0f, 0.1f);
        freqRange.setSkewForCentre (1000.0f);
        params.push_back (std::make_unique<juce::AudioParameterFloat> ("midFreq", "Mid Freq", freqRange, 1000.0f));
    }
    params.push_back (std::make_unique<juce::AudioParameterFloat> (
        "midGain", "Mid Gain",
        juce::NormalisableRange<float> (-12.0f, 12.0f, 0.1f),
        0.0f));
    {
        juce::NormalisableRange<float> qRange (0.1f, 10.0f, 0.01f);
        qRange.setSkewForCentre (1.0f);
        params.push_back (std::make_unique<juce::AudioParameterFloat> ("midQ", "Mid Q", qRange, 1.0f));
    }

    // High Shelf
    {
        juce::NormalisableRange<float> freqRange (1000.0f, 20000.0f, 0.1f);
        freqRange.setSkewForCentre (8000.0f);
        params.push_back (std::make_unique<juce::AudioParameterFloat> ("highFreq", "High Freq", freqRange, 8000.0f));
    }
    params.push_back (std::make_unique<juce::AudioParameterFloat> (
        "highGain", "High Gain",
        juce::NormalisableRange<float> (-12.0f, 12.0f, 0.1f),
        0.0f));
    {
        juce::NormalisableRange<float> qRange (0.1f, 10.0f, 0.01f);
        qRange.setSkewForCentre (1.0f);
        params.push_back (std::make_unique<juce::AudioParameterFloat> ("highQ", "High Q", qRange, 0.707f));
    }

    return { params.begin(), params.end() };
}

//==============================================================================
PluginProcessor::PluginProcessor()
    : AudioProcessor (BusesProperties()
                    #if ! JucePlugin_IsMidiEffect
                     #if ! JucePlugin_IsSynth
                      .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                     #endif
                      .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
                    #endif
                      ),
      apvts (*this, nullptr, "PARAMETERS", createParameterLayout())
{
    rawLowFreq    = apvts.getRawParameterValue ("lowFreq");
    rawLowGain    = apvts.getRawParameterValue ("lowGain");
    rawLowQ       = apvts.getRawParameterValue ("lowQ");
    rawMidFreq    = apvts.getRawParameterValue ("midFreq");
    rawMidGain    = apvts.getRawParameterValue ("midGain");
    rawMidQ       = apvts.getRawParameterValue ("midQ");
    rawHighFreq   = apvts.getRawParameterValue ("highFreq");
    rawHighGain   = apvts.getRawParameterValue ("highGain");
    rawHighQ      = apvts.getRawParameterValue ("highQ");
    rawMasterGain = apvts.getRawParameterValue ("masterGain");
}

PluginProcessor::~PluginProcessor()
{
}

//==============================================================================
const juce::String PluginProcessor::getName() const
{
    return JucePlugin_Name;
}

bool PluginProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool PluginProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool PluginProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double PluginProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int PluginProcessor::getNumPrograms()
{
    return 1;
}

int PluginProcessor::getCurrentProgram()
{
    return 0;
}

void PluginProcessor::setCurrentProgram (int index)
{
    juce::ignoreUnused (index);
}

const juce::String PluginProcessor::getProgramName (int index)
{
    juce::ignoreUnused (index);
    return {};
}

void PluginProcessor::changeProgramName (int index, const juce::String& newName)
{
    juce::ignoreUnused (index, newName);
}

//==============================================================================
void PluginProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    currentSampleRate = sampleRate;

    juce::dsp::ProcessSpec spec;
    spec.sampleRate       = sampleRate;
    spec.maximumBlockSize = static_cast<juce::uint32> (samplesPerBlock);
    spec.numChannels      = 1;

    leftChain.prepare (spec);
    rightChain.prepare (spec);

    smoothedGain.reset (sampleRate, 0.05);
    smoothedGain.setCurrentAndTargetValue (rawMasterGain->load());

    lastParams = {};            // force coefficient update on first processBlock
    updateFilters (readParams());
}

void PluginProcessor::releaseResources()
{
}

bool PluginProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
  #if JucePlugin_IsMidiEffect
    juce::ignoreUnused (layouts);
    return true;
  #else
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
     && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;

   #if ! JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
   #endif

    return true;
  #endif
}

PluginProcessor::FilterParams PluginProcessor::readParams() const noexcept
{
    return { rawLowFreq->load(),  rawLowGain->load(),  rawLowQ->load(),
             rawMidFreq->load(),  rawMidGain->load(),  rawMidQ->load(),
             rawHighFreq->load(), rawHighGain->load(), rawHighQ->load() };
}

void PluginProcessor::writeCoeffsInPlace (juce::dsp::IIR::Coefficients<float>& c,
                                           float b0, float b1, float b2,
                                           float a0, float a1, float a2) noexcept
{
    const float inv = 1.0f / a0;
    auto* raw = c.coefficients.getRawDataPointer();
    raw[0] = b0 * inv;
    raw[1] = b1 * inv;
    raw[2] = b2 * inv;
    raw[3] = a1 * inv;
    raw[4] = a2 * inv;
}

void PluginProcessor::updateFilters (const FilterParams& p)
{
    using namespace juce;
    const float sr = static_cast<float> (currentSampleRate);

    // ── Low Shelf ──────────────────────────────────────────────────────────────
    {
        const float A     = std::sqrt (Decibels::decibelsToGain (p.lowGain));
        const float omega = MathConstants<float>::twoPi * p.lowFreq / sr;
        const float cosW  = std::cos (omega);
        const float sinW  = std::sin (omega);
        const float beta  = sinW * std::sqrt (A) / p.lowQ;

        const float b0 = A * (A + 1.0f - (A - 1.0f) * cosW + beta);
        const float b1 = 2.0f * A * (A - 1.0f - (A + 1.0f) * cosW);
        const float b2 = A * (A + 1.0f - (A - 1.0f) * cosW - beta);
        const float a0 = A + 1.0f + (A - 1.0f) * cosW + beta;
        const float a1 = -2.0f * (A - 1.0f + (A + 1.0f) * cosW);
        const float a2 = A + 1.0f + (A - 1.0f) * cosW - beta;

        writeCoeffsInPlace (*leftChain.get<LowShelf>().coefficients,  b0, b1, b2, a0, a1, a2);
        writeCoeffsInPlace (*rightChain.get<LowShelf>().coefficients, b0, b1, b2, a0, a1, a2);
    }

    // ── Mid Peak/Bell ──────────────────────────────────────────────────────────
    {
        const float A     = std::sqrt (Decibels::decibelsToGain (p.midGain));
        const float omega = MathConstants<float>::twoPi * p.midFreq / sr;
        const float cosW  = std::cos (omega);
        const float sinW  = std::sin (omega);
        const float alpha = sinW / (2.0f * p.midQ);

        const float b0 = 1.0f + alpha * A;
        const float b1 = -2.0f * cosW;
        const float b2 = 1.0f - alpha * A;
        const float a0 = 1.0f + alpha / A;
        const float a1 = -2.0f * cosW;
        const float a2 = 1.0f - alpha / A;

        writeCoeffsInPlace (*leftChain.get<PeakBell>().coefficients,  b0, b1, b2, a0, a1, a2);
        writeCoeffsInPlace (*rightChain.get<PeakBell>().coefficients, b0, b1, b2, a0, a1, a2);
    }

    // ── High Shelf ─────────────────────────────────────────────────────────────
    {
        const float A     = std::sqrt (Decibels::decibelsToGain (p.highGain));
        const float omega = MathConstants<float>::twoPi * p.highFreq / sr;
        const float cosW  = std::cos (omega);
        const float sinW  = std::sin (omega);
        const float beta  = sinW * std::sqrt (A) / p.highQ;

        const float b0 = A * (A + 1.0f + (A - 1.0f) * cosW + beta);
        const float b1 = -2.0f * A * (A - 1.0f + (A + 1.0f) * cosW);
        const float b2 = A * (A + 1.0f + (A - 1.0f) * cosW - beta);
        const float a0 = A + 1.0f - (A - 1.0f) * cosW + beta;
        const float a1 = 2.0f * (A - 1.0f - (A + 1.0f) * cosW);
        const float a2 = A + 1.0f - (A - 1.0f) * cosW - beta;

        writeCoeffsInPlace (*leftChain.get<HighShelf>().coefficients,  b0, b1, b2, a0, a1, a2);
        writeCoeffsInPlace (*rightChain.get<HighShelf>().coefficients, b0, b1, b2, a0, a1, a2);
    }

    lastParams = p;
}

void PluginProcessor::processBlock (juce::AudioBuffer<float>& buffer,
                                    juce::MidiBuffer& midiMessages)
{
    juce::ignoreUnused (midiMessages);

    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels  = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear (i, 0, buffer.getNumSamples());

    // Only recalculate biquad coefficients when a parameter has changed.
    const auto p = readParams();
    if (p != lastParams)
        updateFilters (p);

    juce::dsp::AudioBlock<float> block (buffer);

    auto leftBlock = block.getSingleChannelBlock (0);
    leftChain.process (juce::dsp::ProcessContextReplacing<float> (leftBlock));

    if (totalNumInputChannels > 1)
    {
        auto rightBlock = block.getSingleChannelBlock (1);
        rightChain.process (juce::dsp::ProcessContextReplacing<float> (rightBlock));
    }

    // Apply master gain with per-sample smoothing to avoid clicks.
    smoothedGain.setTargetValue (rawMasterGain->load());
    const int numSamples = buffer.getNumSamples();
    for (int i = 0; i < numSamples; ++i)
    {
        const float gain = smoothedGain.getNextValue();
        for (int ch = 0; ch < totalNumInputChannels; ++ch)
            buffer.getWritePointer (ch)[i] *= gain;
    }
}

//==============================================================================
bool PluginProcessor::hasEditor() const
{
    return true;
}

juce::AudioProcessorEditor* PluginProcessor::createEditor()
{
    return new PluginEditor (*this);
}

//==============================================================================
void PluginProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    auto state = apvts.copyState();
    std::unique_ptr<juce::XmlElement> xml (state.createXml());
    copyXmlToBinary (*xml, destData);
}

void PluginProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    std::unique_ptr<juce::XmlElement> xml (getXmlFromBinary (data, sizeInBytes));
    if (xml != nullptr && xml->hasTagName (apvts.state.getType()))
        apvts.replaceState (juce::ValueTree::fromXml (*xml));
}

//==============================================================================
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new PluginProcessor();
}
