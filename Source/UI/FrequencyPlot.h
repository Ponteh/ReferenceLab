#pragma once

#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_gui_basics/juce_gui_basics.h>
#include <vector>

namespace referencelab::FrequencyPlot {
float frequencyAt(float proportion) noexcept;
float xForFrequency(float frequency, juce::Rectangle<float>) noexcept;
juce::Path spectrumPath(const std::vector<float>& values, int fftSize, double sampleRate,
                        juce::Rectangle<float> area, float minimumDb = -90.0f,
                        float maximumDb = 0.0f, float slopeDbPerOctave = 0.0f);
juce::Path equalizerPath(const juce::AudioProcessorValueTreeState&, double sampleRate,
                         juce::Rectangle<float> area,
                         float minimumDb = -36.0f, float maximumDb = 18.0f);
}
