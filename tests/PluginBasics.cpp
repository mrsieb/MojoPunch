#include "helpers/test_helpers.h"
#include <PluginProcessor.h>
#include <catch2/catch_approx.hpp>
#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_string.hpp>

TEST_CASE ("one is equal to one", "[dummy]")
{
    REQUIRE (1 == 1);
}

TEST_CASE ("Plugin instance", "[instance]")
{
    PluginProcessor testPlugin;

    SECTION ("name")
    {
        CHECK_THAT (testPlugin.getName().toStdString(),
            Catch::Matchers::Equals ("MojoPunch"));
    }
}


TEST_CASE ("Parameter count", "[params]")
{
    PluginProcessor p;
    REQUIRE (p.getParameters().size() == 10);
}

TEST_CASE ("State round-trip", "[state]")
{
    PluginProcessor p;
    p.apvts.getParameter ("lowGain")->setValueNotifyingHost (
        p.apvts.getParameter ("lowGain")->convertTo0to1 (6.0f));

    juce::MemoryBlock state;
    p.getStateInformation (state);

    PluginProcessor p2;
    p2.setStateInformation (state.getData(), (int) state.getSize());

    CHECK (p2.apvts.getRawParameterValue ("lowGain")->load()
           == Catch::Approx (6.0f).epsilon (0.01f));
}

TEST_CASE ("EQ bypass: flat gains -> output equals input times master gain", "[dsp]")
{
    PluginProcessor p;
    p.prepareToPlay (44100.0, 512);

    // Fill both channels with silence, then set an impulse on channel 0
    juce::AudioBuffer<float> buf (2, 512);
    buf.clear();
    buf.setSample (0, 0, 1.0f);
    buf.setSample (1, 0, 1.0f);

    juce::MidiBuffer midi;
    p.processBlock (buf, midi);

    // Default masterGain = 0.5; all EQ gains are 0 dB (unity).
    // The smoothed gain starts AT 0.5, so the very first sample should be 0.5.
    CHECK (buf.getSample (0, 0) == Catch::Approx (0.5f).epsilon (0.05f));
}

#ifdef PAMPLEJUCE_IPP
    #include <ipp.h>

TEST_CASE ("IPP version", "[ipp]")
{
    #if defined(__APPLE__)
        // macOS uses 2021.9.1 from pip wheel (only x86_64 version available)
        CHECK_THAT (ippsGetLibVersion()->Version, Catch::Matchers::Equals ("2021.9.1 (r0x7e208212)"));
    #else
        CHECK_THAT (ippsGetLibVersion()->Version, Catch::Matchers::Equals ("2022.3.0 (r0x0fc08bb1)"));
    #endif
}
#endif
