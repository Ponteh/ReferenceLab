#include "PluginEditor.h"
ReferenceLabEditor::ReferenceLabEditor(ReferenceLabAudioProcessor&x):AudioProcessorEditor(x),p(x){
    setResizable(true,true);setResizeLimits(760,480,1600,1000);setSize(1000,650);
    addAndMakeVisible(add);
    addAndMakeVisible(scan);
    addAndMakeVisible(remove);
    addAndMakeVisible(save);
    addAndMakeVisible(mix);
    addAndMakeVisible(ref);
    addAndMakeVisible(play);
    addAndMakeVisible(stop);
    addAndMakeVisible(title);
    addAndMakeVisible(search);
    addAndMakeVisible(favouritesOnly);
    addAndMakeVisible(editFavourite);
    addAndMakeVisible(sort);
    addAndMakeVisible(cacheSize);
    addAndMakeVisible(autoMatch);
    addAndMakeVisible(matchMode);
    addAndMakeVisible(manualGain);
    addAndMakeVisible(mixMeters);addAndMakeVisible(referenceMeters);addAndMakeVisible(outputMeters);addAndMakeVisible(matchedGain);
    addAndMakeVisible(editTitle);
    addAndMakeVisible(editArtist);
    addAndMakeVisible(editGenre);
    addAndMakeVisible(editBpm);
    addAndMakeVisible(editTags);
    addAndMakeVisible(list);
    title.setText("ReferenceLab 1.0",juce::dontSendNotification);search.setTextToShowWhenEmpty("Cerca titolo, artista, album, genere, tag...",juce::Colours::grey);
    editTitle.setTextToShowWhenEmpty("Titolo",juce::Colours::grey);editArtist.setTextToShowWhenEmpty("Artista",juce::Colours::grey);editGenre.setTextToShowWhenEmpty("Genere",juce::Colours::grey);editBpm.setTextToShowWhenEmpty("BPM",juce::Colours::grey);editTags.setTextToShowWhenEmpty("Tag separati da virgola",juce::Colours::grey);
    sort.addItemList({"Titolo","Artista","Genere","BPM","Rating","Data aggiunta","Ultimo uso"},1);sort.setSelectedId(1,juce::dontSendNotification);
    cacheSize.addItemList({"Cache 128 MB","Cache 256 MB","Cache 512 MB","Cache 1024 MB"},1);cacheSize.setSelectedId(3,juce::dontSendNotification);
    matchMode.addItemList({"Integrated","Short-Term","Momentary"},1);manualGain.setSliderStyle(juce::Slider::LinearHorizontal);manualGain.setTextBoxStyle(juce::Slider::TextBoxRight,false,64,24);manualGain.setTextValueSuffix(" dB");
    autoMatchAttachment=std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(p.state,"autoMatch",autoMatch);matchModeAttachment=std::make_unique<juce::AudioProcessorValueTreeState::ComboBoxAttachment>(p.state,"matchMode",matchMode);manualGainAttachment=std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(p.state,"manualGain",manualGain);
    search.onTextChange=[this]{refreshLibrary();};favouritesOnly.onClick=[this]{refreshLibrary();};sort.onChange=[this]{refreshLibrary();};cacheSize.onChange=[this]{constexpr std::size_t mb=1024u*1024u;const std::size_t sizes[]{128u,256u,512u,1024u};p.getCacheManager().setMaximumBytes(sizes[(size_t)juce::jlimit(0,3,cacheSize.getSelectedId()-1)]*mb);list.repaint();};save.onClick=[this]{saveSelected();};remove.onClick=[this]{removeSelected();};play.onClick=[this]{p.playReference();};stop.onClick=[this]{p.stopReference();};mix.onClick=[this]{p.setReference(false);};ref.onClick=[this]{p.setReference(true);};
    add.onClick=[this]{auto chooser=std::make_shared<juce::FileChooser>("Scegli reference",juce::File{},"*.wav;*.aiff;*.aif;*.flac;*.mp3");chooser->launchAsync(juce::FileBrowserComponent::openMode|juce::FileBrowserComponent::canSelectFiles,[this,chooser](const juce::FileChooser&f){if(f.getResult().existsAsFile()){juce::Component::SafePointer<ReferenceLabEditor>safe(this);p.loadFileAsync(f.getResult(),[safe](const juce::String&error){if(safe){if(error.isNotEmpty())safe->showError(error);safe->refreshLibrary();}});}});};
    scan.onClick=[this]{chooseFolder();};refreshLibrary();startTimerHz(20);
}
void ReferenceLabEditor::refreshLibrary(){referencelab::ReferenceFilter f;f.query=search.getText();f.favouritesOnly=favouritesOnly.getToggleState();f.sort=static_cast<referencelab::ReferenceSort>(juce::jmax(0,sort.getSelectedId()-1));rows=p.getReferenceManager().filter(f);list.updateContent();list.repaint();}
void ReferenceLabEditor::showError(const juce::String&e){juce::NativeMessageBox::showMessageBoxAsync(juce::MessageBoxIconType::WarningIcon,"ReferenceLab",e);}
void ReferenceLabEditor::chooseFolder(){auto chooser=std::make_shared<juce::FileChooser>("Cartella reference");chooser->launchAsync(juce::FileBrowserComponent::openMode|juce::FileBrowserComponent::canSelectDirectories,[this,chooser](const juce::FileChooser&f){if(f.getResult().isDirectory()){juce::String warning;p.getReferenceManager().scanFolder(f.getResult(),true,warning);if(warning.isNotEmpty())showError(warning);refreshLibrary();}});}
void ReferenceLabEditor::paintListBoxItem(int row,juce::Graphics&g,int w,int h,bool selected){if(row<0||row>=getNumRows())return;if(selected)g.fillAll(juce::Colour(0xff315b78));auto lib=p.getReferenceManager().snapshot();auto file=rows[(size_t)row].resolveAgainst(lib.root);auto state=p.getCacheManager().getState(file);juce::String badge=state==referencelab::CacheState::cached?"[CACHE] ":state==referencelab::CacheState::loading?"[LOAD] ":state==referencelab::CacheState::error?"[ERR] ":"";g.setColour(file.existsAsFile()?juce::Colours::white:juce::Colours::orangered);g.drawText(badge+rows[(size_t)row].title+"  -  "+rows[(size_t)row].artist,10,0,w-20,h,juce::Justification::centredLeft,true);}
void ReferenceLabEditor::listBoxItemDoubleClicked(int row,const juce::MouseEvent&){if(row<0||row>=getNumRows())return;auto file=rows[(size_t)row].resolveAgainst(p.getReferenceManager().snapshot().root);juce::Component::SafePointer<ReferenceLabEditor>safe(this);p.loadFileAsync(file,[safe](const juce::String&error){if(!safe)return;if(error.isNotEmpty())safe->showError(error);else safe->p.setReference(true);safe->list.repaint();});list.repaint();}
void ReferenceLabEditor::selectedRowsChanged(int row){if(row<0||row>=getNumRows())return;auto&m=rows[(size_t)row];editTitle.setText(m.title,false);editArtist.setText(m.artist,false);editGenre.setText(m.genre,false);editBpm.setText(m.bpm?juce::String(*m.bpm,2):juce::String{},false);editTags.setText(m.tags.joinIntoString(", "),false);editFavourite.setToggleState(m.favourite,juce::dontSendNotification);}
void ReferenceLabEditor::saveSelected(){auto row=list.getSelectedRow();if(row<0||row>=getNumRows())return;auto item=rows[(size_t)row];item.title=editTitle.getText().trim();item.artist=editArtist.getText().trim();item.genre=editGenre.getText().trim();item.favourite=editFavourite.getToggleState();item.tags=juce::StringArray::fromTokens(editTags.getText(),",","\"");item.tags.trim();item.tags.removeEmptyStrings();auto bpm=editBpm.getText().trim();item.bpm=bpm.isEmpty()?std::nullopt:std::optional<double>(bpm.getDoubleValue());juce::String error;if(!p.getReferenceManager().updateMetadata(item,error))showError(error);refreshLibrary();}
void ReferenceLabEditor::removeSelected(){auto row=list.getSelectedRow();if(row<0||row>=getNumRows())return;juce::String error;if(!p.getReferenceManager().remove(rows[(size_t)row].uuid,error))showError(error);refreshLibrary();}
void ReferenceLabEditor::timerCallback(){auto text=[](const char*name,const referencelab::MeterSnapshot&m){return juce::String(name)+"  LUFS "+juce::String(m.shortTermLufs,1)+"  Peak "+juce::String(m.peakDb,1)+"  TP "+juce::String(m.truePeakDbtp,1)+"  RMS "+juce::String(m.rmsDb,1)+"  Crest "+juce::String(m.crestDb,1);};mixMeters.setText(text("MIX",p.getMixMeters()),juce::dontSendNotification);referenceMeters.setText(text("REF",p.getReferenceMeters()),juce::dontSendNotification);auto o=p.getOutputMeters();outputMeters.setText("OUT  Corr "+juce::String(o.correlation,2)+"  Width "+juce::String(o.stereoWidth,2),juce::dontSendNotification);matchedGain.setText("Applied "+juce::String(p.getMatchedGainDb(),1)+" dB",juce::dontSendNotification);list.repaint();}
void ReferenceLabEditor::paint(juce::Graphics&g){g.fillAll(juce::Colour(0xff10151d));g.setColour(juce::Colour(0xff263241));g.fillRoundedRectangle(getLocalBounds().reduced(16).toFloat(),10);}
void ReferenceLabEditor::resized(){auto r=getLocalBounds().reduced(28);title.setBounds(r.removeFromTop(38));auto bar=r.removeFromTop(36);add.setBounds(bar.removeFromLeft(120));scan.setBounds(bar.removeFromLeft(160).reduced(4,0));remove.setBounds(bar.removeFromLeft(90).reduced(4,0));ref.setBounds(bar.removeFromRight(105));mix.setBounds(bar.removeFromRight(75));auto filters=r.removeFromTop(36);search.setBounds(filters.removeFromLeft(330));favouritesOnly.setBounds(filters.removeFromLeft(125));sort.setBounds(filters.removeFromLeft(130));cacheSize.setBounds(filters.removeFromLeft(140).reduced(4,0));auto bottom=r.removeFromBottom(190);auto editor=bottom.removeFromTop(76);auto line=editor.removeFromTop(34);editTitle.setBounds(line.removeFromLeft(230));editArtist.setBounds(line.removeFromLeft(210).reduced(4,0));editGenre.setBounds(line.removeFromLeft(150).reduced(4,0));editBpm.setBounds(line.removeFromLeft(90).reduced(4,0));auto line2=editor.removeFromTop(36);editTags.setBounds(line2.removeFromLeft(400));editFavourite.setBounds(line2.removeFromLeft(100));save.setBounds(line2.removeFromLeft(130));play.setBounds(line2.removeFromLeft(65).reduced(3,0));stop.setBounds(line2.removeFromLeft(65).reduced(3,0));auto match=bottom.removeFromTop(36);autoMatch.setBounds(match.removeFromLeft(110));matchMode.setBounds(match.removeFromLeft(130));manualGain.setBounds(match.removeFromLeft(220));matchedGain.setBounds(match.removeFromLeft(130));mixMeters.setBounds(bottom.removeFromTop(24));referenceMeters.setBounds(bottom.removeFromTop(24));outputMeters.setBounds(bottom.removeFromTop(24));r.removeFromTop(8);list.setBounds(r);}
