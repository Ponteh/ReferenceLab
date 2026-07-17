#include "FrequencyPlot.h"
#include "../State/ComparisonState.h"
#include <cmath>

namespace referencelab::FrequencyPlot {
float frequencyAt(float proportion) noexcept {
    return 20.0f * std::pow(1000.0f, juce::jlimit(0.0f, 1.0f, proportion));
}

float xForFrequency(float frequency, juce::Rectangle<float> area) noexcept {
    return area.getX() + std::log10(juce::jlimit(20.0f, 20000.0f, frequency) / 20.0f)
                           / 3.0f * area.getWidth();
}

juce::Path spectrumPath(const std::vector<float>& values, int fftSize, double sampleRate,
                        juce::Rectangle<float> area, float minimumDb, float maximumDb) {
    juce::Path path;
    if (values.empty() || fftSize <= 0 || sampleRate <= 0.0) return path;
    for (int x = 0; x < (int)area.getWidth(); ++x) {
        const auto proportion = x / juce::jmax(1.0f, area.getWidth() - 1.0f);
        const auto frequency = frequencyAt(proportion);
        const auto bin = juce::jlimit(1, (int)values.size() - 1,
                                     (int)(frequency / sampleRate * fftSize));
        const auto y = juce::jmap(juce::jlimit(minimumDb, maximumDb, values[(size_t)bin]),
                                 minimumDb, maximumDb, area.getBottom(), area.getY());
        if (x == 0) path.startNewSubPath(area.getX(), y);
        else path.lineTo(area.getX() + x, y);
    }
    return path;
}

juce::Path equalizerPath(const juce::AudioProcessorValueTreeState& state,
                         double sampleRate,juce::Rectangle<float> area,
                         float minimumDb,float maximumDb) {
    juce::Path path;
    const auto settings = comparisonSettingsFromState(state);
    if (settings.bypass) return path;
    for (int x = 0; x < (int)area.getWidth(); ++x) {
        const auto proportion = x / juce::jmax(1.0f, area.getWidth() - 1.0f);
        const auto y = juce::jmap(juce::jlimit(minimumDb,maximumDb,
                                              ComparisonProcessor::responseDecibelsAt(
                                                  settings,frequencyAt(proportion),sampleRate)),
                                 minimumDb, maximumDb, area.getBottom(), area.getY());
        if (x == 0) path.startNewSubPath(area.getX(), y);
        else path.lineTo(area.getX() + x, y);
    }
    return path;
}
}
