#pragma once
#include <juce_dsp/juce_dsp.h>
#include <juce_gui_basics/juce_gui_basics.h>
#include <juce_events/juce_events.h>
#include "../Audio/SampleFifo.h"
#include <array>

namespace referencelab {
class AnalysisDisplay final:public juce::Component,private juce::Timer{
public:AnalysisDisplay(SampleFifo&mix,SampleFifo&reference,SampleFifo&output);
    void paint(juce::Graphics&)override;
    void setFrozen(bool value)noexcept{frozen=value;}
private:
    static constexpr int order=11,size=1<<order;
    SampleFifo&mixFifo,&referenceFifo,&outputFifo;juce::dsp::FFT fft{order};juce::dsp::WindowingFunction<float>window{size,juce::dsp::WindowingFunction<float>::hann};bool frozen=false;
    std::array<float,size>mixTime{},referenceTime{},outputTime{},scratch{};std::array<float,size*2>mixFft{},referenceFft{};std::array<float,size/2>mixSpectrum{},referenceSpectrum{};
    void timerCallback()override;void consume(SampleFifo&,std::array<float,size>&);void calculate(const std::array<float,size>&,std::array<float,size*2>&,std::array<float,size/2>&);juce::Path spectrumPath(const std::array<float,size/2>&,juce::Rectangle<float>)const;
};
}
