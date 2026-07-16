#pragma once
#include <juce_gui_extra/juce_gui_extra.h>
#include "PluginProcessor.h"
#include "UI/AnalysisDisplay.h"

class ReferenceLabEditor final:public juce::AudioProcessorEditor,private juce::ListBoxModel,private juce::Timer{
public: explicit ReferenceLabEditor(ReferenceLabAudioProcessor&);void paint(juce::Graphics&)override;void resized()override;bool keyPressed(const juce::KeyPress&)override;
private:
    int getNumRows() override{return (int)rows.size();}
    void paintListBoxItem(int,juce::Graphics&,int,int,bool) override;
    void listBoxItemDoubleClicked(int,const juce::MouseEvent&) override;
    void selectedRowsChanged(int) override;
    void timerCallback() override;
    void refreshLibrary(); void chooseFolder(); void showError(const juce::String&); void saveSelected(); void removeSelected();void configurePlayback(int);void applyLoop();void persistPlayback();
    ReferenceLabAudioProcessor&p;referencelab::AnalysisDisplay analysisDisplay;juce::TextButton add{"Aggiungi file"},scan{"Scansiona cartella"},remove{"Rimuovi"},relink{"Ricollega"},save{"Salva metadati"},savePreset{"Salva preset"},loadPreset{"Carica preset"},resetMeters{"Reset meter"},mix{"MIX"},ref{"REFERENCE"},play{"Play"},pause{"Pause"},stop{"Stop"};
    juce::Label title,mixMeters,referenceMeters,outputMeters,matchedGain,transportStatus;juce::TextEditor search,filterArtist,filterGenre,filterKey,minBpm,maxBpm,editTitle,editArtist,editAlbum,editGenre,editYear,editBpm,editKey,editRating,editTags,editNotes;juce::ToggleButton recursiveScan{"Sottocartelle"},favouritesOnly{"Solo preferiti"},editFavourite{"Preferita"},autoMatch{"Auto Match"},matchBypass{"Match Bypass"},freezeDisplay{"Freeze"},safeExport{"Safe Export"},loopEnabled{"Loop"},eqBypass{"EQ Bypass"},hpfEnabled{"HPF"},bellEnabled{"Bell"},lpfEnabled{"LPF"},transportSync{"Sync DAW"};juce::ComboBox sort,cacheSize,matchMode,listeningMode,hpfSlope,lpfSlope,fftSize,displayRate;juce::Slider manualGain,seek,startOffset,loopStart,loopEnd,fade,hpf,bellFreq,bellGain,bellQ,lpf,syncOffset,fftSmoothing;juce::ListBox list{"Reference library",this};
    std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment>autoMatchAttachment;std::unique_ptr<juce::AudioProcessorValueTreeState::ComboBoxAttachment>matchModeAttachment;std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment>manualGainAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment>eqBypassAttachment;std::unique_ptr<juce::AudioProcessorValueTreeState::ComboBoxAttachment>listeningModeAttachment;std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment>fadeAttachment,hpfAttachment,bellFreqAttachment,bellGainAttachment,bellQAttachment,lpfAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment>transportSyncAttachment;std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment>syncOffsetAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment>matchBypassAttachment,safeExportAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment>hpfEnabledAttachment,bellEnabledAttachment,lpfEnabledAttachment;std::unique_ptr<juce::AudioProcessorValueTreeState::ComboBoxAttachment>hpfSlopeAttachment,lpfSlopeAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ComboBoxAttachment>fftSizeAttachment,displayRateAttachment;std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment>fftSmoothingAttachment;
    std::vector<referencelab::ReferenceMetadata> rows;
};
