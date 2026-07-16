#pragma once
#include <juce_gui_extra/juce_gui_extra.h>
#include "PluginProcessor.h"

class ReferenceLabEditor final:public juce::AudioProcessorEditor,private juce::ListBoxModel{
public: explicit ReferenceLabEditor(ReferenceLabAudioProcessor&);void paint(juce::Graphics&)override;void resized()override;
private:
    int getNumRows() override{return (int)rows.size();}
    void paintListBoxItem(int,juce::Graphics&,int,int,bool) override;
    void listBoxItemDoubleClicked(int,const juce::MouseEvent&) override;
    void refreshLibrary(); void chooseFolder(); void showError(const juce::String&);
    ReferenceLabAudioProcessor&p;juce::TextButton add{"Aggiungi file"},scan{"Scansiona cartella"},mix{"MIX"},ref{"REFERENCE"},play{"Play"},stop{"Stop"};
    juce::Label title;juce::TextEditor search;juce::ListBox list{"Reference library",this};
    std::vector<referencelab::ReferenceMetadata> rows;
};
