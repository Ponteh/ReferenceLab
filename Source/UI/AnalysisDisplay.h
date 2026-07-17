#pragma once
#include "../Audio/SpectrumAnalyzer.h"
#include <juce_gui_basics/juce_gui_basics.h>
#include <juce_events/juce_events.h>
#include <juce_audio_processors/juce_audio_processors.h>
#include <array>

namespace referencelab {
class AnalysisDisplay final:public juce::Component,private juce::Timer{
public:
    AnalysisDisplay(SampleFifo&,SampleFifo&,SampleFifo&,juce::AudioProcessorValueTreeState&);void paint(juce::Graphics&)override;
    void mouseMove(const juce::MouseEvent&)override;void mouseDrag(const juce::MouseEvent&)override;void mouseExit(const juce::MouseEvent&)override;
    void setSampleRate(double value)noexcept{sampleRate=value>0?value:44100.0;}
    void setFrozen(bool value)noexcept{frozen=value;}void setFftOrder(int);void setSmoothing(float value)noexcept{mixAnalyzer.setSmoothing(value);referenceAnalyzer.setSmoothing(value);}void setRefreshRate(int hz){startTimerHz(juce::jlimit(10,60,hz));}
private:
    static constexpr int maxOrder=12,maxSize=1<<maxOrder;
    SampleFifo&mixFifo,&referenceFifo,&outputFifo;juce::AudioProcessorValueTreeState&state;double sampleRate=44100.0;SpectrumAnalyzer mixAnalyzer{11},referenceAnalyzer{11};bool frozen=false;
    std::array<float,maxSize>outputTime{},scratch{};juce::Point<float>cursorPosition;bool cursorVisible=false;
    void timerCallback()override;void consume(SampleFifo&,std::array<float,maxSize>&);void paintCursor(juce::Graphics&);
};
}
