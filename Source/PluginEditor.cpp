#include "PluginEditor.h"
ReferenceLabEditor::ReferenceLabEditor(ReferenceLabAudioProcessor& x):AudioProcessorEditor(x),p(x){
    setResizable(true,true); setSize(960,620);
    for(auto* c:{&add,&mix,&ref}) addAndMakeVisible(c); addAndMakeVisible(title);
    title.setText("ReferenceLab 1.0 - Local Reference Core",juce::dontSendNotification);
    add.onClick=[this]{
        auto chooser=std::make_shared<juce::FileChooser>("Scegli reference",juce::File{},"*.wav;*.aiff;*.aif;*.flac;*.mp3");
        chooser->launchAsync(juce::FileBrowserComponent::openMode|juce::FileBrowserComponent::canSelectFiles,[this,chooser](const juce::FileChooser& f){
            if(f.getResult().existsAsFile()){juce::String error;if(!p.loadFile(f.getResult(),error))juce::NativeMessageBox::showMessageBoxAsync(juce::MessageBoxIconType::WarningIcon,"ReferenceLab",error);}
        });
    };
    mix.onClick=[this]{p.setReference(false);}; ref.onClick=[this]{p.setReference(true);};
}
void ReferenceLabEditor::paint(juce::Graphics&g){g.fillAll(juce::Colour(0xff10151d));g.setColour(juce::Colour(0xff263241));g.fillRoundedRectangle(getLocalBounds().reduced(18).toFloat(),12);g.setColour(juce::Colours::white.withAlpha(.7f));g.drawText("LIBRARY     ACTIVE REFERENCE     COMPARISON EQ     FFT / METERS",30,90,getWidth()-60,40,juce::Justification::centred);}
void ReferenceLabEditor::resized(){auto r=getLocalBounds().reduced(30);title.setBounds(r.removeFromTop(40));auto top=r.removeFromTop(42);add.setBounds(top.removeFromLeft(220));mix.setBounds(top.removeFromRight(120));ref.setBounds(top.removeFromRight(140));}
