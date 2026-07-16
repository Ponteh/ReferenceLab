#include "ReferencePlayer.h"
#include <cmath>

namespace referencelab {
bool ReferencePlayer::load(const juce::File& file, juce::AudioFormatManager& formats, juce::String& error) {
    std::unique_ptr<juce::AudioFormatReader> reader(formats.createReaderFor(file));
    if (!reader) { error = "Formato audio non supportato o file non valido"; return false; }
    if (reader->lengthInSamples <= 0 || reader->lengthInSamples > std::numeric_limits<int>::max()) { error = "Durata file non supportata"; return false; }
    auto next = std::make_shared<ReferenceAudioData>();
    next->sampleRate = reader->sampleRate;
    next->samples.setSize(2, static_cast<int>(reader->lengthInSamples));
    if (!reader->read(&next->samples, 0, next->samples.getNumSamples(), 0, true, true)) { error = "Errore durante la decodifica"; return false; }
    if (reader->numChannels == 1) next->samples.copyFrom(1, 0, next->samples, 0, 0, next->samples.getNumSamples());
    setAudio(std::move(next)); return true;
}
void ReferencePlayer::setAudio(std::shared_ptr<ReferenceAudioData> next) noexcept { audio.store(std::move(next),std::memory_order_release);position.store(startOffset.load()); }
void ReferencePlayer::prepare(double rate) noexcept { hostRate.store(rate > 0.0 ? rate : 44100.0); }
void ReferencePlayer::stop() noexcept { playing.store(false); position.store(startOffset.load()); }
void ReferencePlayer::seek(double seconds) noexcept { position.store(juce::jmax(0.0, seconds)); }
void ReferencePlayer::setStartOffset(double seconds) noexcept { startOffset.store(juce::jmax(0.0, seconds)); }
void ReferencePlayer::setLoop(bool enabled, double start, double end, double crossfadeMs) noexcept { loopStart.store(juce::jmax(0.0,start)); loopEnd.store(juce::jmax(0.0,end));loopCrossfade.store(juce::jlimit(0.0,0.1,crossfadeMs/1000.0));loopEnabled.store(enabled && end > start); }
double ReferencePlayer::getPositionSeconds() const noexcept { return position.load(); }
double ReferencePlayer::getDurationSeconds() const noexcept { auto d=audio.load(std::memory_order_acquire); return d ? d->samples.getNumSamples()/d->sampleRate : 0.0; }
void ReferencePlayer::process(juce::AudioBuffer<float>& out) noexcept {
    out.clear(); auto data=audio.load(std::memory_order_acquire); if (!data || !playing.load()) return;
    auto pos=position.load(); const auto duration=getDurationSeconds(); const auto step=data->sampleRate/hostRate.load();
    for(int i=0;i<out.getNumSamples();++i) {
        const bool looping=loopEnabled.load();
        const double loopA=loopStart.load();
        const double loopB=loopEnd.load();
        if(looping&&pos>=loopB)pos=loopA+std::fmod(pos-loopB,loopB-loopA);
        if(pos>=duration) { playing.store(false); break; }
        const auto sourcePos=pos*data->sampleRate; const auto index=static_cast<int>(sourcePos); const auto fraction=static_cast<float>(sourcePos-index);
        const auto next=juce::jmin(index+1,data->samples.getNumSamples()-1);
        for(int c=0;c<juce::jmin(2,out.getNumChannels());++c){auto sample=juce::jmap(fraction,data->samples.getSample(c,index),data->samples.getSample(c,next));auto fade=juce::jmin(loopCrossfade.load(),(loopB-loopA)*.5);if(looping&&fade>0.0&&pos>loopB-fade){auto blend=(float)((pos-(loopB-fade))/fade);auto fadePos=juce::jlimit(0.0,(double)data->samples.getNumSamples()-1.0,(loopA+pos-(loopB-fade))*data->sampleRate);auto fadeIndex=(int)fadePos;auto fadeNext=juce::jmin(fadeIndex+1,data->samples.getNumSamples()-1);auto startSample=juce::jmap((float)(fadePos-fadeIndex),data->samples.getSample(c,fadeIndex),data->samples.getSample(c,fadeNext));sample=juce::jmap(blend,sample,startSample);}out.setSample(c,i,sample);}
        pos += step/data->sampleRate;
    }
    position.store(pos);
}
}
