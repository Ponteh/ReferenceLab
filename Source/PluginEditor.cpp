#include "PluginEditor.h"
ReferenceLabEditor::ReferenceLabEditor(ReferenceLabAudioProcessor&x):AudioProcessorEditor(x),p(x){
    setResizable(true,true);setResizeLimits(760,480,1600,1000);setSize(1000,650);
    addAndMakeVisible(add);
    addAndMakeVisible(scan);
    addAndMakeVisible(mix);
    addAndMakeVisible(ref);
    addAndMakeVisible(play);
    addAndMakeVisible(stop);
    addAndMakeVisible(title);
    addAndMakeVisible(search);
    addAndMakeVisible(list);
    title.setText("ReferenceLab 1.0",juce::dontSendNotification);search.setTextToShowWhenEmpty("Cerca titolo, artista, album, genere, tag...",juce::Colours::grey);
    search.onTextChange=[this]{refreshLibrary();};play.onClick=[this]{p.playReference();};stop.onClick=[this]{p.stopReference();};mix.onClick=[this]{p.setReference(false);};ref.onClick=[this]{p.setReference(true);};
    add.onClick=[this]{auto chooser=std::make_shared<juce::FileChooser>("Scegli reference",juce::File{},"*.wav;*.aiff;*.aif;*.flac;*.mp3");chooser->launchAsync(juce::FileBrowserComponent::openMode|juce::FileBrowserComponent::canSelectFiles,[this,chooser](const juce::FileChooser&f){if(f.getResult().existsAsFile()){juce::String error;if(!p.loadFile(f.getResult(),error))showError(error);refreshLibrary();}});};
    scan.onClick=[this]{chooseFolder();};refreshLibrary();
}
void ReferenceLabEditor::refreshLibrary(){rows=p.getReferenceManager().search(search.getText());list.updateContent();list.repaint();}
void ReferenceLabEditor::showError(const juce::String&e){juce::NativeMessageBox::showMessageBoxAsync(juce::MessageBoxIconType::WarningIcon,"ReferenceLab",e);}
void ReferenceLabEditor::chooseFolder(){auto chooser=std::make_shared<juce::FileChooser>("Cartella reference");chooser->launchAsync(juce::FileBrowserComponent::openMode|juce::FileBrowserComponent::canSelectDirectories,[this,chooser](const juce::FileChooser&f){if(f.getResult().isDirectory()){juce::String warning;p.getReferenceManager().scanFolder(f.getResult(),true,warning);if(warning.isNotEmpty())showError(warning);refreshLibrary();}});}
void ReferenceLabEditor::paintListBoxItem(int row,juce::Graphics&g,int w,int h,bool selected){if(row<0||row>=getNumRows())return;if(selected)g.fillAll(juce::Colour(0xff315b78));auto lib=p.getReferenceManager().snapshot();auto file=rows[(size_t)row].resolveAgainst(lib.root);g.setColour(file.existsAsFile()?juce::Colours::white:juce::Colours::orangered);g.drawText(rows[(size_t)row].title+"  -  "+rows[(size_t)row].artist,10,0,w-20,h,juce::Justification::centredLeft,true);}
void ReferenceLabEditor::listBoxItemDoubleClicked(int row,const juce::MouseEvent&){if(row<0||row>=getNumRows())return;auto file=rows[(size_t)row].resolveAgainst(p.getReferenceManager().snapshot().root);juce::String error;if(!p.loadFile(file,error))showError(error);else p.setReference(true);}
void ReferenceLabEditor::paint(juce::Graphics&g){g.fillAll(juce::Colour(0xff10151d));g.setColour(juce::Colour(0xff263241));g.fillRoundedRectangle(getLocalBounds().reduced(16).toFloat(),10);}
void ReferenceLabEditor::resized(){auto r=getLocalBounds().reduced(28);title.setBounds(r.removeFromTop(38));auto bar=r.removeFromTop(36);add.setBounds(bar.removeFromLeft(130));scan.setBounds(bar.removeFromLeft(170).reduced(4,0));search.setBounds(bar.removeFromLeft(330).reduced(4,0));ref.setBounds(bar.removeFromRight(110));mix.setBounds(bar.removeFromRight(85));auto controls=r.removeFromBottom(42);play.setBounds(controls.removeFromLeft(90));stop.setBounds(controls.removeFromLeft(90).reduced(4,0));r.removeFromTop(10);list.setBounds(r);}
