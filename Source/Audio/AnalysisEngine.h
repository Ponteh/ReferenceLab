#pragma once
#include <juce_audio_basics/juce_audio_basics.h>
#include <atomic>

namespace referencelab {
struct MeterSnapshot { float peakDb=-100.f,truePeakDbtp=-100.f,rmsDb=-100.f,crestDb=0.f,momentaryLufs=-100.f,shortTermLufs=-100.f,integratedLufs=-100.f,correlation=0.f,stereoWidth=0.f; };
class AnalysisEngine {
public:
    void prepare(double sampleRate) noexcept; void reset() noexcept; void process(const juce::AudioBuffer<float>&) noexcept; MeterSnapshot snapshot()const noexcept;
private:
    double rate=44100.0,integratedEnergy=0.0,integratedSamples=0.0;float momentaryEnergy=0.f,shortEnergy=0.f,previousL=0.f,previousR=0.f;
    std::atomic<float> peak{-100.f},truePeak{-100.f},rms{-100.f},crest{0.f},momentary{-100.f},shortTerm{-100.f},integrated{-100.f},correlation{0.f},width{0.f};
    static float db(float value)noexcept;static float lufs(double energy)noexcept;
};
}
