#include "EqCurveDisplay.h"
#include "FrequencyPlot.h"
#include "UiTheme.h"

namespace referencelab {
EqCurveDisplay::EqCurveDisplay(SampleFifo&mixMid,SampleFifo&referenceMid,SampleFifo&mixSide,SampleFifo&referenceSide,juce::AudioProcessorValueTreeState&parameters)
    :mixMidFifo(mixMid),referenceMidFifo(referenceMid),mixSideFifo(mixSide),referenceSideFifo(referenceSide),state(parameters) {
    startTimerHz(30);
}

void EqCurveDisplay::timerCallback() {
    mixMidAnalyzer.update(mixMidFifo);referenceMidAnalyzer.update(referenceMidFifo);mixSideAnalyzer.update(mixSideFifo);referenceSideAnalyzer.update(referenceSideFifo);
    repaint();
}

void EqCurveDisplay::paint(juce::Graphics& g) {
    auto area = getLocalBounds().toFloat();
    g.setColour(juce::Colour(0xff151d27));
    g.fillRoundedRectangle(area, 6.0f);
    area.removeFromLeft(48.f);area.removeFromBottom(22.f);area.reduce(8.0f, 4.0f);
    g.setFont(10.0f);

    for (auto db : { 0, -30, -60, -90 }) {
        const auto y = juce::jmap((float)db, -90.0f, 0.0f, area.getBottom(), area.getY());
        g.setColour(juce::Colours::white.withAlpha(0.09f));
        g.drawHorizontalLine((int)y, area.getX(), area.getRight());
        g.setColour(juce::Colours::white.withAlpha(0.45f));
        g.drawText(juce::String(db) + " dB", 2, (int)y - 6, 43, 12,juce::Justification::right);
    }
    for (auto frequency : { 20, 50, 100, 200, 500, 1000, 2000, 5000, 10000, 20000 }) {
        const auto x = FrequencyPlot::xForFrequency((float)frequency, area);
        g.setColour(juce::Colours::white.withAlpha(0.07f));
        g.drawVerticalLine((int)x, area.getY(), area.getBottom());
        g.setColour(juce::Colours::white.withAlpha(0.4f));
        g.drawText(frequency >= 1000 ? juce::String(frequency / 1000) + "k" : juce::String(frequency),
                   (int)x - 14, (int)area.getBottom() + 4, 28, 12, juce::Justification::centred);
    }

    const auto mixColour=UiTheme::mix(state.state),referenceColour=UiTheme::reference(state.state);
    const auto slope=state.getRawParameterValue("analysisSlope")->load();
    const bool showMid=state.getRawParameterValue("showMidSpectrum")->load()>.5f,showSide=state.getRawParameterValue("showSideSpectrum")->load()>.5f;
    auto draw=[&](const SpectrumAnalyzer&analyzer,juce::Colour colour,bool selected,float fillAlpha,float lineAlpha)
    {
        auto path=FrequencyPlot::spectrumPath(analyzer.getSpectrum(),analyzer.getFftSize(),sampleRate,area,-90.f,0.f,slope);
        if(selected){auto fill=path;fill.lineTo(area.getRight(),area.getBottom());fill.lineTo(area.getX(),area.getBottom());fill.closeSubPath();g.setColour(colour.withAlpha(fillAlpha));g.fillPath(fill);}
        g.setColour(colour.withAlpha(lineAlpha));g.strokePath(path,juce::PathStrokeType(selected?1.8f:1.35f));
    };
    if(showMid){draw(mixMidAnalyzer,mixColour,!referenceSelected,.14f,referenceSelected ? .72f : 1.f);if(referenceAvailable)draw(referenceMidAnalyzer,referenceColour,referenceSelected,.14f,referenceSelected ? 1.f : .72f);}
    if(showSide){draw(mixSideAnalyzer,mixColour,!referenceSelected,.25f,referenceSelected ? .58f : .86f);if(referenceAvailable)draw(referenceSideAnalyzer,referenceColour,referenceSelected,.25f,referenceSelected ? .86f : .58f);}

    const auto bypassed = state.getRawParameterValue("eqBypass")->load() > 0.5f;
    if (!bypassed) {
        auto curve = FrequencyPlot::equalizerPath(state, sampleRate, area);
        g.setColour(UiTheme::equalizer());
        g.strokePath(curve, juce::PathStrokeType(2.0f));
    }

    auto legend = area.toNearestInt().removeFromTop(18);
    g.setColour(mixColour);g.drawText(referenceSelected?"MIX outline":"MIX selected",legend.removeFromLeft(82),juce::Justification::left);
    g.setColour(referenceColour);g.drawText(!referenceAvailable?"REF unavailable":referenceSelected?"REF selected":"REF outline",legend.removeFromLeft(92),juce::Justification::left);
    g.setColour(juce::Colours::white.withAlpha(.72f));g.drawText(!showMid&&!showSide?"NO LAYERS":showMid&&showSide?"MID + SIDE":showMid?"MID":"SIDE",legend.removeFromLeft(75),juce::Justification::left);
    g.setColour(bypassed ? juce::Colours::white.withAlpha(0.55f) : UiTheme::equalizer());
    g.drawText(bypassed ? "EQ BYPASSED" : "EQ RESPONSE", legend, juce::Justification::right);
}
}
