#pragma once
#include <juce_dsp/juce_dsp.h>
#include <juce_gui_basics/juce_gui_basics.h>
#include <juce_events/juce_events.h>
#include <juce_audio_processors/juce_audio_processors.h>
#include "../Audio/SampleFifo.h"
#include <array>
#include <memory>

namespace referencelab {
class AnalysisDisplay final:public juce::Component,private juce::Timer{
public:
    AnalysisDisplay(SampleFifo&,SampleFifo&,SampleFifo&,juce::AudioProcessorValueTreeState&);void paint(juce::Graphics&)override;
    void setSampleRate(double value)noexcept{sampleRate=value>0?value:44100.0;}
    void setFrozen(bool value)noexcept{frozen=value;}void setFftOrder(int);void setSmoothing(float value)noexcept{smoothing=juce::jlimit(.01f,1.f,value);}void setRefreshRate(int hz){startTimerHz(juce::jlimit(10,60,hz));}
private:
    static constexpr int maxOrder=12,maxSize=1<<maxOrder;
    SampleFifo&mixFifo,&referenceFifo,&outputFifo;juce::AudioProcessorValueTreeState&state;double sampleRate=44100.0;std::unique_ptr<juce::dsp::FFT>fft;std::unique_ptr<juce::dsp::WindowingFunction<float>>window;int currentOrder=11,currentSize=1<<11;float smoothing=.22f;bool frozen=false;
    std::array<float,maxSize>mixTime{},referenceTime{},outputTime{},scratch{};std::array<float,maxSize*2>mixFft{},referenceFft{};std::array<float,maxSize/2>mixSpectrum{},referenceSpectrum{};
    void timerCallback()override;void consume(SampleFifo&,std::array<float,maxSize>&);void calculate(const std::array<float,maxSize>&,std::array<float,maxSize*2>&,std::array<float,maxSize/2>&);juce::Path spectrumPath(const std::array<float,maxSize/2>&,juce::Rectangle<float>)const;juce::Path eqPath(juce::Rectangle<float>)const;float eqDbAt(float)const;
};
}
