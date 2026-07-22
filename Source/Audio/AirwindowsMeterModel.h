#pragma once
#include <juce_audio_basics/juce_audio_basics.h>
#include <array>
#include <atomic>
#include <cmath>

namespace referencelab {
// Analysis adapted from Airwindows Meter by Chris Johnson.
struct AirwindowsMeterFrame {
    float rmsL=0,rmsR=0,peakL=0,peakR=0,slewL=0,slewR=0,zeroL=0,zeroR=0;
    bool reference=false,boundary=false;
};

class AirwindowsMeterModel {
public:
    void prepare(double)noexcept;
    void reset()noexcept;
    void setSource(bool isReference)noexcept;
    void markBoundary()noexcept{boundaryPending.store(true,std::memory_order_release);}
    void requestReferenceScoreReset()noexcept{referenceScoreResetPending.store(true,std::memory_order_release);}
    void requestScoreReset()noexcept{mixScoreResetPending.store(true,std::memory_order_release);requestReferenceScoreReset();}
    void process(const juce::AudioBuffer<float>&)noexcept;
    void processScores(const juce::AudioBuffer<float>&mix,const juce::AudioBuffer<float>&reference,bool referenceActive)noexcept;
    int pull(AirwindowsMeterFrame*,int)noexcept;
    int pullScores(AirwindowsMeterFrame*,int)noexcept;

private:
    struct Accumulator {
        double rmsL=0,rmsR=0;
        float peakL=0,peakR=0,slewL=0,slewR=0,previousL=0,previousR=0;
        float zeroL=0,zeroR=0,longestZeroL=0,longestZeroR=0;
        bool wasPositiveL=false,wasPositiveR=false;
        int count=0;
        void reset()noexcept;
        void clearWindow()noexcept;
    };

    static constexpr int displayCapacity=256,scoreCapacity=512;
    juce::AbstractFifo displayFifo{displayCapacity},scoreFifo{scoreCapacity};
    std::array<AirwindowsMeterFrame,displayCapacity>displayFrames{};
    std::array<AirwindowsMeterFrame,scoreCapacity>scoreFrames{};
    Accumulator displayAccumulator;
    std::array<Accumulator,2>scoreAccumulators{};
    double sampleRate=44100.0;
    std::atomic<bool>sourceReference{false},boundaryPending{true};
    std::atomic<bool>mixScoreResetPending{true},referenceScoreResetPending{true};
    std::array<bool,2>scoreResetMarkers{{true,true}};
    bool referenceScoreWasActive=false;

    void processBuffer(const juce::AudioBuffer<float>&,Accumulator&,bool score,bool referenceSource)noexcept;
    bool pushDisplay(const AirwindowsMeterFrame&)noexcept;
    bool pushScore(const AirwindowsMeterFrame&)noexcept;
};
}
