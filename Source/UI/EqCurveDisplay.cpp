#include "EqCurveDisplay.h"
#include "FrequencyPlot.h"
#include "UiTheme.h"

namespace referencelab {
EqCurveDisplay::EqCurveDisplay(SampleFifo& source, juce::AudioProcessorValueTreeState& parameters)
    : fifo(source), state(parameters) {
    startTimerHz(30);
}

void EqCurveDisplay::timerCallback() {
    analyzer.update(fifo);
    repaint();
}

void EqCurveDisplay::paint(juce::Graphics& g) {
    auto area = getLocalBounds().toFloat();
    g.setColour(juce::Colour(0xff151d27));
    g.fillRoundedRectangle(area, 6.0f);
    area.reduce(10.0f, 8.0f);
    g.setFont(10.0f);

    for (auto db : { 0, -30, -60, -90 }) {
        const auto y = juce::jmap((float)db, -90.0f, 0.0f, area.getBottom(), area.getY());
        g.setColour(juce::Colours::white.withAlpha(0.09f));
        g.drawHorizontalLine((int)y, area.getX(), area.getRight());
        g.setColour(juce::Colours::white.withAlpha(0.45f));
        g.drawText(juce::String(db) + " dB", (int)area.getX() + 2, (int)y - 12, 42, 12,
                   juce::Justification::left);
    }
    for (auto frequency : { 20, 50, 100, 200, 500, 1000, 2000, 5000, 10000, 20000 }) {
        const auto x = FrequencyPlot::xForFrequency((float)frequency, area);
        g.setColour(juce::Colours::white.withAlpha(0.07f));
        g.drawVerticalLine((int)x, area.getY(), area.getBottom());
        g.setColour(juce::Colours::white.withAlpha(0.4f));
        g.drawText(frequency >= 1000 ? juce::String(frequency / 1000) + "k" : juce::String(frequency),
                   (int)x - 14, (int)area.getBottom() - 13, 28, 12, juce::Justification::centred);
    }

    if (audioAvailable) {
        auto audio = FrequencyPlot::spectrumPath(analyzer.getSpectrum(), analyzer.getFftSize(), sampleRate, area);
        auto fill = audio;
        fill.lineTo(area.getRight(), area.getBottom());
        fill.lineTo(area.getX(), area.getBottom());
        fill.closeSubPath();
        g.setGradientFill(juce::ColourGradient(audioColour.withAlpha(.44f), area.getX(), area.getY(),
                                              audioColour.withAlpha(.06f), area.getX(), area.getBottom(), false));
        g.fillPath(fill);
        g.setColour(audioColour);
        g.strokePath(audio, juce::PathStrokeType(1.3f));
    }

    const auto bypassed = state.getRawParameterValue("eqBypass")->load() > 0.5f;
    if (!bypassed) {
        auto curve = FrequencyPlot::equalizerPath(state, sampleRate, area);
        g.setColour(UiTheme::equalizer());
        g.strokePath(curve, juce::PathStrokeType(2.0f));
    }

    auto legend = area.toNearestInt().removeFromTop(18);
    g.setColour(audioColour);
    g.drawText(audioAvailable ? "POST-EQ AUDIO" : "NESSUNA REFERENCE", legend.removeFromLeft(125), juce::Justification::left);
    g.setColour(bypassed ? juce::Colours::white.withAlpha(0.55f) : UiTheme::equalizer());
    g.drawText(bypassed ? "EQ BYPASSED" : "EQ RESPONSE", legend, juce::Justification::right);
}
}
