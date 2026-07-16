#pragma once
#include <juce_audio_basics/juce_audio_basics.h>
#include <array>
#include <atomic>
#include <cstdint>

namespace referencelab {
struct MeterSnapshot { float peakDb=-100.f,truePeakDbtp=-100.f,rmsDb=-100.f,crestDb=0.f,momentaryLufs=-100.f,shortTermLufs=-100.f,integratedLufs=-100.f,dynamicRangeDb=0.f,correlation=0.f,stereoWidth=0.f; };
class AnalysisEngine {
public:
    void prepare(double sampleRate)noexcept;void reset()noexcept{resetRequested.store(true);}void process(const juce::AudioBuffer<float>&)noexcept;MeterSnapshot snapshot()const noexcept;
private:
    static constexpr int histogramBins=1200;
    struct Biquad{double b0=1,b1=0,b2=0,a1=0,a2=0,z1=0,z2=0;double process(double x)noexcept{auto y=b0*x+z1;z1=b1*x-a1*y+z2;z2=b2*x-a2*y;return y;}void set(double nb0,double nb1,double nb2,double a0,double na1,double na2)noexcept{b0=nb0/a0;b1=nb1/a0;b2=nb2/a0;a1=na1/a0;a2=na2/a0;}void clear()noexcept{z1=z2=0;}};
    double rate=44100.0,chunkEnergy=0;int chunkSamples=0,chunkTarget=4410,chunkIndex=0,shortIndex=0,validChunks=0;std::array<double,4>momentChunks{};std::array<double,30>shortChunks{};std::array<std::uint32_t,histogramBins>gateCounts{},shortCounts{};std::array<double,histogramBins>gateEnergy{};Biquad shelf[2],rlb[2];float previous[2][4]{};int historyCount=0;
    std::atomic<bool>resetRequested{false};std::atomic<float>peak{-100.f},truePeak{-100.f},rms{-100.f},crest{0.f},momentary{-100.f},shortTerm{-100.f},integrated{-100.f},dynamicRange{0.f},correlation{0.f},width{0.f};
    static float db(float)noexcept;static float lufs(double)noexcept;static int binFor(float)noexcept;void configureKWeighting()noexcept;void doReset()noexcept;void finishChunk()noexcept;float integratedFromGate()const noexcept;float loudnessRange(float integratedValue)const noexcept;
};
}
