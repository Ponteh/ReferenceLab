#include "LoudnessMatcher.h"
namespace referencelab {
void LoudnessMatcher::prepare(double sr)noexcept{
    for(auto*g:{&mixGain,&referenceGain}){g->reset(sr,.35);g->setCurrentAndTargetValue(1.f);}
}
void LoudnessMatcher::process(juce::AudioBuffer<float>&mixBuffer,juce::AudioBuffer<float>&referenceBuffer,
                              const MeterSnapshot&m,const MeterSnapshot&r,bool enabled,LoudnessMode mode,
                              float targetLufs,float manual,float limit)noexcept{
    auto pick=[&](const MeterSnapshot&s){return mode==LoudnessMode::integrated?s.integratedLufs:mode==LoudnessMode::shortTerm?s.shortTermLufs:s.momentaryLufs;};
    auto correction=[&](float measured,float additional){return enabled&&measured>-80.f?juce::jlimit(-std::abs(limit),std::abs(limit),targetLufs-measured)+additional:additional;};
    const auto mixDb=juce::jlimit(-24.f,24.f,correction(pick(m),0.f));
    const auto referenceDb=juce::jlimit(-24.f,24.f,correction(pick(r),manual));
    mixGain.setTargetValue(juce::Decibels::decibelsToGain(mixDb));
    referenceGain.setTargetValue(juce::Decibels::decibelsToGain(referenceDb));
    const auto samples=juce::jmin(mixBuffer.getNumSamples(),referenceBuffer.getNumSamples());
    for(int i=0;i<samples;++i){
        const auto mg=mixGain.getNextValue(),rg=referenceGain.getNextValue();
        for(int c=0;c<mixBuffer.getNumChannels();++c)mixBuffer.setSample(c,i,mixBuffer.getSample(c,i)*mg);
        for(int c=0;c<referenceBuffer.getNumChannels();++c)referenceBuffer.setSample(c,i,referenceBuffer.getSample(c,i)*rg);
    }
    appliedMixDb.store(juce::Decibels::gainToDecibels(mixGain.getCurrentValue()));
    appliedReferenceDb.store(juce::Decibels::gainToDecibels(referenceGain.getCurrentValue()));
}
}
