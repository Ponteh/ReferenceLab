#include "HeadphoneCorrection.h"

namespace referencelab {
void HeadphoneCorrection::prepare(double sr,int){sampleRate=sr>0?sr:44100.0;active.reset();pending.store({});}
void HeadphoneCorrection::setProfile(const std::optional<HeadphoneProfile>&profile){
    if(!profile){pendingClear.store(true);return;}
    auto engine=std::make_shared<Engine>();engine->preamp=juce::Decibels::decibelsToGain(profile->preampDb);engine->stages.resize(profile->filters.size());
    for(size_t i=0;i<profile->filters.size();++i){auto&filter=profile->filters[i];const auto frequency=juce::jlimit(10.f,(float)(sampleRate*.49),filter.frequency);juce::dsp::IIR::Coefficients<float>::Ptr coefficients;switch(filter.type){case HeadphoneFilterType::lowShelf:coefficients=juce::dsp::IIR::Coefficients<float>::makeLowShelf(sampleRate,frequency,filter.q,juce::Decibels::decibelsToGain(filter.gainDb));break;case HeadphoneFilterType::highShelf:coefficients=juce::dsp::IIR::Coefficients<float>::makeHighShelf(sampleRate,frequency,filter.q,juce::Decibels::decibelsToGain(filter.gainDb));break;default:coefficients=juce::dsp::IIR::Coefficients<float>::makePeakFilter(sampleRate,frequency,filter.q,juce::Decibels::decibelsToGain(filter.gainDb));break;}for(auto&channel:engine->stages[i].channels)channel.coefficients=coefficients;}
    pending.store(std::move(engine));
}
void HeadphoneCorrection::process(juce::AudioBuffer<float>&buffer)noexcept{
    if(pendingClear.exchange(false))active.reset();
    if(auto next=pending.exchange({}))active=std::move(next);
    if(!enabled.load()||!active)return;
    buffer.applyGain(active->preamp);
    for(auto&stage:active->stages)for(int channel=0;channel<juce::jmin(2,buffer.getNumChannels());++channel){auto*data=buffer.getWritePointer(channel);for(int sample=0;sample<buffer.getNumSamples();++sample)data[sample]=stage.channels[(size_t)channel].processSample(data[sample]);}
}
}
