#pragma once
#include "AnalysisEngine.h"
#include <juce_audio_basics/juce_audio_basics.h>
namespace referencelab {
enum class LoudnessMode{integrated,shortTerm,momentary};
class LoudnessMatcher{
public:void prepare(double sampleRate)noexcept;void process(juce::AudioBuffer<float>&reference,const MeterSnapshot&mix,const MeterSnapshot&ref,bool enabled,LoudnessMode,float manualDb,float limitDb)noexcept;float getAppliedGainDb()const noexcept{return appliedDb.load();}
private:juce::SmoothedValue<float,juce::ValueSmoothingTypes::Linear>gain;std::atomic<float>appliedDb{0.f};
};
}
