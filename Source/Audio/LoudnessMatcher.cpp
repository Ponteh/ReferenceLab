#include "LoudnessMatcher.h"
namespace referencelab {
void LoudnessMatcher::prepare(double sr)noexcept{gain.reset(sr,.35);gain.setCurrentAndTargetValue(1.f);}
void LoudnessMatcher::process(juce::AudioBuffer<float>&b,const MeterSnapshot&m,const MeterSnapshot&r,bool enabled,LoudnessMode mode,float manual,float limit)noexcept{auto pick=[&](const MeterSnapshot&s){return mode==LoudnessMode::integrated?s.integratedLufs:mode==LoudnessMode::shortTerm?s.shortTermLufs:s.momentaryLufs;};auto mix=pick(m),ref=pick(r);float automatic=enabled&&mix>-80.f&&ref>-80.f?juce::jlimit(-std::abs(limit),std::abs(limit),mix-ref):0.f;auto target=juce::jlimit(-24.f,24.f,automatic+manual);gain.setTargetValue(juce::Decibels::decibelsToGain(target));for(int i=0;i<b.getNumSamples();++i){auto g=gain.getNextValue();for(int c=0;c<b.getNumChannels();++c)b.setSample(c,i,b.getSample(c,i)*g);}appliedDb.store(juce::Decibels::gainToDecibels(gain.getCurrentValue()));}
}
