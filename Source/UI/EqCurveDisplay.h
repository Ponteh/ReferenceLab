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
    void setAudioColour(juce::Colour value) noexcept
    {
        if (audioColour != value) { audioColour = value; repaint(); }
    }
    void setAudioAvailable(bool value) noexcept
    {
        if (audioAvailable != value) { audioAvailable = value; repaint(); }
    }

private:
    SampleFifo& fifo;
    juce::AudioProcessorValueTreeState& state;
    SpectrumAnalyzer analyzer{11};
    juce::Colour audioColour{0xff54c6eb};
    bool audioAvailable = true;
    double sampleRate = 44100.0;

    void timerCallback() override;
};
}
