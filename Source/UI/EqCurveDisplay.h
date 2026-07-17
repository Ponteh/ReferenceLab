#pragma once

#include "../Audio/SampleFifo.h"
#include "../Audio/SpectrumAnalyzer.h"
#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_events/juce_events.h>
#include <juce_gui_basics/juce_gui_basics.h>

namespace referencelab {
class EqCurveDisplay final : public juce::Component, private juce::Timer {
public:
    EqCurveDisplay(SampleFifo&, juce::AudioProcessorValueTreeState&);
    void paint(juce::Graphics&) override;
    void setSampleRate(double value) noexcept { sampleRate = value > 0.0 ? value : 44100.0; }

private:
    SampleFifo& fifo;
    juce::AudioProcessorValueTreeState& state;
    SpectrumAnalyzer analyzer{11};
    double sampleRate = 44100.0;

    void timerCallback() override;
};
}
