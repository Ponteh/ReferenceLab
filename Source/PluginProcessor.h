#pragma once
#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_audio_utils/juce_audio_utils.h>
#include <foleys_gui_magic/foleys_gui_magic.h>
#include <cstdint>
#include "Audio/ComparisonProcessor.h"
#include "Audio/CacheManager.h"
#include "Audio/AnalysisEngine.h"
#include "Audio/LoudnessMatcher.h"
#include "Audio/SampleFifo.h"
#include "Audio/ReferencePlayer.h"
#include "Audio/TransportController.h"
#include "Audio/AirwindowsMeterModel.h"
#include "Library/ReferenceManager.h"

class ReferenceLabAudioProcessor final : public juce::AudioProcessor {
public:
    enum class RemoteLoadState { idle,downloading,decoding,ready,error };
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
    bool loadFile(const juce::File&,juce::String&); void setReference(bool b){reference.store(b);airwindowsMeterModel.setSource(b&&player.isLoaded());} bool isReference()const{return reference.load();}
    void loadFileAsync(const juce::File&,std::function<void(const juce::String&)>);
    void loadUrlAsync(const juce::URL&,std::function<void(const juce::String&)>);
    void importCatalogUrlAsync(const juce::URL&,std::function<void(int,const juce::String&)>);
    RemoteLoadState getRemoteLoadState()const noexcept{return remoteLoadState.load();}float getRemoteLoadProgress()const noexcept{return remoteLoadProgress.load();}
    bool saveComparisonPreset(const juce::File&,juce::String&);bool loadComparisonPreset(const juce::File&,juce::String&);
    void playReference(){player.play();} void pauseReference(){player.pause();} void stopReference(){player.stop();}
    referencelab::ReferencePlayer& getPlayer(){return player;}
    referencelab::ReferenceManager& getReferenceManager(){return manager;}
    referencelab::CacheManager& getCacheManager(){return cache;}
    referencelab::MeterSnapshot getMixMeters()const{return mixAnalysis.snapshot();}
    referencelab::MeterSnapshot getReferenceMeters()const{return referenceAnalysis.snapshot();}
    referencelab::MeterSnapshot getOutputMeters()const{return outputAnalysis.snapshot();}
    float getMatchedGainDb()const{return matcher.getAppliedGainDb();}
    bool isTransportAvailable()const{return transportAvailable.load();}
    bool isHostPlaying()const noexcept{return hostPlaying.load();}double getHostPositionSeconds()const noexcept{return hostPositionSeconds.load();}double getHostBpm()const noexcept{return hostBpm.load();}int getHostTimeSignatureNumerator()const noexcept{return hostTimeSignatureNumerator.load();}int getHostTimeSignatureDenominator()const noexcept{return hostTimeSignatureDenominator.load();}
    foleys::MagicProcessorState&getGuiState()noexcept{return guiState;}
    void resetMeters()noexcept{mixAnalysis.reset();referenceAnalysis.reset();outputAnalysis.reset();}
    referencelab::AirwindowsMeterModel&getAirwindowsMeterModel()noexcept{return airwindowsMeterModel;}
    referencelab::SampleFifo&getMixFifo(){return mixFifo;}referencelab::SampleFifo&getReferenceFifo(){return referenceFifo;}referencelab::SampleFifo&getMixSideFifo(){return mixSideFifo;}referencelab::SampleFifo&getReferenceSideFifo(){return referenceSideFifo;}referencelab::SampleFifo&getOutputFifo(){return outputFifo;}
    referencelab::SampleFifo&getCompareMixMidFifo(){return compareMixMidFifo;}referencelab::SampleFifo&getCompareReferenceMidFifo(){return compareReferenceMidFifo;}referencelab::SampleFifo&getCompareMixSideFifo(){return compareMixSideFifo;}referencelab::SampleFifo&getCompareReferenceSideFifo(){return compareReferenceSideFifo;}
    juce::AudioProcessorValueTreeState state;
private:
    static juce::AudioProcessorValueTreeState::ParameterLayout createLayout();
    foleys::MagicProcessorState guiState{*this};juce::AudioFormatManager formats; referencelab::ReferenceManager manager; referencelab::CacheManager cache; referencelab::ReferencePlayer player; referencelab::ComparisonProcessor comparison;referencelab::AnalysisEngine mixAnalysis,referenceAnalysis,outputAnalysis;referencelab::LoudnessMatcher matcher;referencelab::AirwindowsMeterModel airwindowsMeterModel;
    juce::AudioBuffer<float> referenceBuffer;std::atomic<bool>reference{false};float blendCurrent=0.f,blendTarget=0.f,blendStep=0.f;int blendRemaining=0;
    referencelab::SampleFifo mixFifo,referenceFifo,mixSideFifo,referenceSideFifo,outputFifo,compareMixMidFifo,compareReferenceMidFifo,compareMixSideFifo,compareReferenceSideFifo;
    referencelab::TransportController transportController;std::atomic<bool>hostPlaying{false};std::atomic<double>hostPositionSeconds{-1.0},hostBpm{0.0};std::atomic<int>hostTimeSignatureNumerator{0},hostTimeSignatureDenominator{0};
    mutable juce::CriticalSection activeSourceLock;juce::String activeSource;std::atomic<double>pendingRestorePosition{-1.0};std::atomic<bool>transportAvailable{false};
    std::shared_ptr<int> lifetimeToken{std::make_shared<int>(0)};juce::ThreadPool remoteDownloadPool{1};std::atomic<RemoteLoadState>remoteLoadState{RemoteLoadState::idle};std::atomic<float>remoteLoadProgress{0};std::atomic<std::uint64_t>remoteLoadGeneration{0},catalogImportGeneration{0};
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ReferenceLabAudioProcessor)
};
