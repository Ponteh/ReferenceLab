#pragma once
#include "AnalysisEngine.h"
#include <juce_audio_basics/juce_audio_basics.h>
namespace referencelab {
enum class LoudnessMode{integrated,shortTerm,momentary};
class LoudnessMatcher{
public:
    void prepare(double sampleRate)noexcept;
    void process(juce::AudioBuffer<float>&mixBuffer,juce::AudioBuffer<float>&referenceBuffer,
                 const MeterSnapshot&mix,const MeterSnapshot&ref,bool enabled,LoudnessMode,
                 float targetLufs,float manualDb,float limitDb)noexcept;
    float getMixGainDb()const noexcept{return appliedMixDb.load();}
    float getReferenceGainDb()const noexcept{return appliedReferenceDb.load();}
private:
    juce::SmoothedValue<float,juce::ValueSmoothingTypes::Linear>mixGain,referenceGain;
    std::atomic<float>appliedMixDb{0.f},appliedReferenceDb{0.f};
};
}
