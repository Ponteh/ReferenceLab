#pragma once
#include <juce_audio_basics/juce_audio_basics.h>
#include <array>
#include <cmath>

namespace referencelab {
// Analysis adapted from Airwindows Meter by Chris Johnson.
struct AirwindowsMeterFrame { float rmsL=0,rmsR=0,peakL=0,peakR=0,slewL=0,slewR=0,bassL=0,bassR=0; };
class AirwindowsMeterModel {
public:
    void prepare(double value)noexcept{sampleRate=value>0?value:44100.0;reset();}
    void reset()noexcept;
    void process(const juce::AudioBuffer<float>&)noexcept;
    int pull(AirwindowsMeterFrame*,int)noexcept;
private:
    static constexpr int capacity=256;juce::AbstractFifo fifo{capacity};std::array<AirwindowsMeterFrame,capacity>frames{};double sampleRate=44100.0,rmsL=0,rmsR=0;float peakL=0,peakR=0,slewL=0,slewR=0,previousL=0,previousR=0,bassL=0,bassR=0;int count=0;void push(const AirwindowsMeterFrame&)noexcept;
};
}
