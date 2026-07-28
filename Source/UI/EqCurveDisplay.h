#pragma once

#include "../Audio/SampleFifo.h"
#include "../Audio/SpectrumAnalyzer.h"
#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_events/juce_events.h>
#include <juce_gui_basics/juce_gui_basics.h>
#include <array>

namespace referencelab {
class EqCurveDisplay final : public juce::Component, private juce::Timer {
public:
    EqCurveDisplay(SampleFifo& mixMid,SampleFifo& referenceMid,SampleFifo& mixSide,SampleFifo& referenceSide,
                   SampleFifo& mixLeft,SampleFifo& mixRight,SampleFifo& referenceLeft,SampleFifo& referenceRight,
                   juce::AudioProcessorValueTreeState&);
    void paint(juce::Graphics&) override;
    void setFftOrder(int order){mixMidAnalyzer.setFftOrder(order);referenceMidAnalyzer.setFftOrder(order);mixSideAnalyzer.setFftOrder(order);referenceSideAnalyzer.setFftOrder(order);}
    void setSampleRate(double value) noexcept { sampleRate = value > 0.0 ? value : 44100.0; }
    void setReferenceSelected(bool value)noexcept{if(referenceSelected!=value){referenceSelected=value;repaint();}}
    void setReferenceAvailable(bool value)noexcept{if(referenceAvailable!=value){referenceAvailable=value;repaint();}}

private:
    static constexpr int fieldSize=512;
    SampleFifo&mixMidFifo,&referenceMidFifo,&mixSideFifo,&referenceSideFifo,&mixLeftFifo,&mixRightFifo,&referenceLeftFifo,&referenceRightFifo;
    juce::AudioProcessorValueTreeState& state;
    SpectrumAnalyzer mixMidAnalyzer{11},referenceMidAnalyzer{11},mixSideAnalyzer{11},referenceSideAnalyzer{11};
    bool referenceSelected=false,referenceAvailable=false;
    double sampleRate = 44100.0;
    std::array<float,fieldSize>mixLeft{},mixRight{},referenceLeft{},referenceRight{},scratch{};

    void timerCallback() override;
    void consume(SampleFifo&,std::array<float,fieldSize>&);
};
}
