#pragma once
#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_audio_utils/juce_audio_utils.h>
#include "Audio/ComparisonProcessor.h"
#include "Audio/CacheManager.h"
#include "Audio/ReferencePlayer.h"
#include "Library/ReferenceManager.h"

class ReferenceLabAudioProcessor final : public juce::AudioProcessor {
public:
    ReferenceLabAudioProcessor(); ~ReferenceLabAudioProcessor() override;
    void prepareToPlay(double,int) override; void releaseResources() override {}
    bool isBusesLayoutSupported(const BusesLayout&) const override;
    void processBlock(juce::AudioBuffer<float>&,juce::MidiBuffer&) override;
    juce::AudioProcessorEditor* createEditor() override; bool hasEditor() const override{return true;}
    const juce::String getName() const override{return "ReferenceLab";}
    bool acceptsMidi()const override{return false;} bool producesMidi()const override{return false;} bool isMidiEffect()const override{return false;}
    double getTailLengthSeconds()const override{return 0;} int getNumPrograms()override{return 1;} int getCurrentProgram()override{return 0;}
    void setCurrentProgram(int)override{} const juce::String getProgramName(int)override{return{};} void changeProgramName(int,const juce::String&)override{}
    void getStateInformation(juce::MemoryBlock&)override; void setStateInformation(const void*,int)override;
    bool loadFile(const juce::File&,juce::String&); void setReference(bool b){reference.store(b);} bool isReference()const{return reference.load();}
    void loadFileAsync(const juce::File&,std::function<void(const juce::String&)>);
    void playReference(){player.play();} void pauseReference(){player.pause();} void stopReference(){player.stop();}
    referencelab::ReferencePlayer& getPlayer(){return player;}
    referencelab::ReferenceManager& getReferenceManager(){return manager;}
    referencelab::CacheManager& getCacheManager(){return cache;}
    juce::AudioProcessorValueTreeState state;
private:
    static juce::AudioProcessorValueTreeState::ParameterLayout createLayout();
    juce::AudioFormatManager formats; referencelab::ReferenceManager manager; referencelab::CacheManager cache; referencelab::ReferencePlayer player; referencelab::ComparisonProcessor comparison;
    juce::AudioBuffer<float> referenceBuffer; std::atomic<bool> reference{false}; juce::SmoothedValue<float> blend;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ReferenceLabAudioProcessor)
};
