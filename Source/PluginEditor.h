#pragma once
#include <juce_gui_extra/juce_gui_extra.h>
#include "PluginProcessor.h"

class ReferenceLabEditor final:public juce::AudioProcessorEditor,private juce::ListBoxModel,private juce::Timer{
public: explicit ReferenceLabEditor(ReferenceLabAudioProcessor&);void paint(juce::Graphics&)override;void resized()override;
private:
    int getNumRows() override{return (int)rows.size();}
    void paintListBoxItem(int,juce::Graphics&,int,int,bool) override;
    void listBoxItemDoubleClicked(int,const juce::MouseEvent&) override;
    void selectedRowsChanged(int) override;
    void timerCallback() override;
    void refreshLibrary(); void chooseFolder(); void showError(const juce::String&); void saveSelected(); void removeSelected();
    ReferenceLabAudioProcessor&p;juce::TextButton add{"Aggiungi file"},scan{"Scansiona cartella"},remove{"Rimuovi"},save{"Salva metadati"},mix{"MIX"},ref{"REFERENCE"},play{"Play"},stop{"Stop"};
    juce::Label title,mixMeters,referenceMeters,outputMeters,matchedGain;juce::TextEditor search,editTitle,editArtist,editGenre,editBpm,editTags;juce::ToggleButton favouritesOnly{"Solo preferiti"},editFavourite{"Preferita"},autoMatch{"Auto Match"};juce::ComboBox sort,cacheSize,matchMode;juce::Slider manualGain;juce::ListBox list{"Reference library",this};
    std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment>autoMatchAttachment;std::unique_ptr<juce::AudioProcessorValueTreeState::ComboBoxAttachment>matchModeAttachment;std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment>manualGainAttachment;
    std::vector<referencelab::ReferenceMetadata> rows;
};
