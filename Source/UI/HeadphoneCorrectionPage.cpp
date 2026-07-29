#include "HeadphoneCorrectionPage.h"
#include "../PluginProcessor.h"
#include <juce_dsp/juce_dsp.h>

namespace referencelab {
namespace {
void setContextHelp(juce::Component&component,const juce::String&text)
{
    component.getProperties().set("referenceLabContextHelp",text);
    if(auto*client=dynamic_cast<juce::SettableTooltipClient*>(&component))client->setTooltip({});
}

void configureKnob(juce::Slider&slider,const juce::String&suffix){
    slider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    slider.setTextBoxStyle(juce::Slider::TextBoxBelow,false,72,22);
    slider.setTextValueSuffix(suffix);
    slider.setScrollWheelEnabled(false);
    slider.setColour(juce::Slider::rotarySliderFillColourId,juce::Colours::white);
    slider.setColour(juce::Slider::rotarySliderOutlineColourId,juce::Colour(0xff405365));
    slider.setColour(juce::Slider::thumbColourId,juce::Colours::white);
    slider.setColour(juce::Slider::textBoxTextColourId,juce::Colours::white);
    slider.setColour(juce::Slider::textBoxBackgroundColourId,juce::Colour(0xff101923));
    slider.setColour(juce::Slider::textBoxOutlineColourId,juce::Colour(0xff405365));
}
void configureToggle(juce::TextButton&button){
    button.setColour(juce::TextButton::buttonColourId,juce::Colour(0xff26313b));
    button.setColour(juce::TextButton::buttonOnColourId,juce::Colours::white);
    button.setColour(juce::TextButton::textColourOffId,juce::Colours::white);
    button.setColour(juce::TextButton::textColourOnId,juce::Colour(0xff10151d));
}
}

HeadphoneCorrectionPage::BandEditor::BandEditor(HeadphoneFilter&value,int index,std::function<void()>edited):filter(value),onEdited(std::move(edited)){
    title.setText("BAND "+juce::String(index+1),juce::dontSendNotification);title.setFont(juce::FontOptions(13.f,juce::Font::bold));
    type.addItemList({"Peaking","Low Shelf","High Shelf"},1);
    type.setSelectedId(filter.type==HeadphoneFilterType::lowShelf?2:filter.type==HeadphoneFilterType::highShelf?3:1,juce::dontSendNotification);
    configureKnob(frequency," Hz");configureKnob(gain," dB");configureKnob(q,{});
    frequency.setRange(10,40000,1);frequency.setSkewFactorFromMidPoint(1000);frequency.setValue(filter.frequency,juce::dontSendNotification);
    gain.setRange(-30,30,.1);gain.setValue(filter.gainDb,juce::dontSendNotification);q.setRange(.05,30,.01);q.setValue(filter.q,juce::dontSendNotification);
    setContextHelp(type,"Choose the headphone filter type.");setContextHelp(frequency,"Set the headphone filter frequency.");setContextHelp(gain,"Set the headphone filter gain.");setContextHelp(q,"Set the headphone filter bandwidth.");
    type.onChange=[this]{capture();};frequency.onValueChange=[this]{capture();};gain.onValueChange=[this]{capture();};q.onValueChange=[this]{capture();};
    addAndMakeVisible(title);addAndMakeVisible(type);addAndMakeVisible(frequency);addAndMakeVisible(gain);addAndMakeVisible(q);
}
void HeadphoneCorrectionPage::BandEditor::capture(){filter.type=type.getSelectedId()==2?HeadphoneFilterType::lowShelf:type.getSelectedId()==3?HeadphoneFilterType::highShelf:HeadphoneFilterType::peaking;filter.frequency=(float)frequency.getValue();filter.gainDb=(float)gain.getValue();filter.q=(float)q.getValue();if(onEdited)onEdited();}
void HeadphoneCorrectionPage::BandEditor::paint(juce::Graphics&g){g.setColour(juce::Colour(0xff16212c));g.fillRoundedRectangle(getLocalBounds().toFloat(),7.f);g.setColour(juce::Colour(0xff304356));g.drawRoundedRectangle(getLocalBounds().toFloat().reduced(.5f),7.f,1.f);}
void HeadphoneCorrectionPage::BandEditor::resized(){auto area=getLocalBounds().reduced(8);auto top=area.removeFromTop(28);title.setBounds(top.removeFromLeft(76));type.setBounds(top);auto width=area.getWidth()/3;frequency.setBounds(area.removeFromLeft(width));gain.setBounds(area.removeFromLeft(width));q.setBounds(area);}

void HeadphoneCorrectionPage::CurveDisplay::paint(juce::Graphics&g){
    g.fillAll(juce::Colour(0xff151d27));auto area=getLocalBounds().toFloat();g.setColour(juce::Colour(0xff304356));g.drawRoundedRectangle(area.reduced(.5f),6.f,1.f);area.removeFromLeft(42);area.removeFromBottom(20);area.reduce(8,5);g.setFont(10.f);
    for(auto db:{18,12,6,0,-6,-12,-18}){auto y=juce::jmap((float)db,-18.f,18.f,area.getBottom(),area.getY());g.setColour(juce::Colours::white.withAlpha(db==0?.18f:.07f));g.drawHorizontalLine((int)y,area.getX(),area.getRight());g.setColour(juce::Colours::white.withAlpha(.48f));g.drawText(juce::String(db),2,(int)y-6,34,12,juce::Justification::right);}
    for(auto hz:{20,50,100,200,500,1000,2000,5000,10000,20000}){auto proportion=(float)(std::log10((double)hz/20.0)/3.0);auto x=area.getX()+proportion*area.getWidth();g.setColour(juce::Colours::white.withAlpha(.06f));g.drawVerticalLine((int)x,area.getY(),area.getBottom());g.setColour(juce::Colours::white.withAlpha(.45f));g.drawText(hz>=1000?juce::String(hz/1000)+"k":juce::String(hz),(int)x-14,(int)area.getBottom()+4,28,12,juce::Justification::centred);}
    if(!profile)return;std::vector<juce::dsp::IIR::Coefficients<float>::Ptr>coefficients;for(auto&filter:profile->filters){auto frequency=juce::jlimit(10.f,(float)(sampleRate*.49),filter.frequency);switch(filter.type){case HeadphoneFilterType::lowShelf:coefficients.push_back(juce::dsp::IIR::Coefficients<float>::makeLowShelf(sampleRate,frequency,filter.q,juce::Decibels::decibelsToGain(filter.gainDb)));break;case HeadphoneFilterType::highShelf:coefficients.push_back(juce::dsp::IIR::Coefficients<float>::makeHighShelf(sampleRate,frequency,filter.q,juce::Decibels::decibelsToGain(filter.gainDb)));break;default:coefficients.push_back(juce::dsp::IIR::Coefficients<float>::makePeakFilter(sampleRate,frequency,filter.q,juce::Decibels::decibelsToGain(filter.gainDb)));break;}}
    juce::Path path;for(int x=0;x<(int)area.getWidth();++x){auto frequency=20.0*std::pow(1000.0,(double)x/juce::jmax(1.f,area.getWidth()-1));auto db=profile->preampDb;for(auto&c:coefficients)db+=(float)juce::Decibels::gainToDecibels(c->getMagnitudeForFrequency(frequency,sampleRate));auto y=juce::jmap(juce::jlimit(-18.f,18.f,db),-18.f,18.f,area.getBottom(),area.getY());if(x==0)path.startNewSubPath(area.getX(),y);else path.lineTo(area.getX()+x,y);}g.setColour(juce::Colours::white.withAlpha(.95f));g.strokePath(path,juce::PathStrokeType(1.8f));g.drawText("HEADPHONE CORRECTION RESPONSE",area.toNearestInt().removeFromTop(18),juce::Justification::left);
}

HeadphoneCorrectionPage::HeadphoneCorrectionPage(ReferenceLabAudioProcessor&p):processor(p){
    heading.setText("AUTOEQ HEADPHONE CORRECTION",juce::dontSendNotification);heading.setFont(juce::FontOptions(18.f,juce::Font::bold));
    status.setText("Search AutoEq online or load a saved profile offline.",juce::dontSendNotification);
    search.setTextToShowWhenEmpty("Search headphone model...",juce::Colours::grey);
    profileNameHint.setText("PROFILE NAME — optional; leave empty to save the original headphone name",juce::dontSendNotification);profileNameHint.setFont(juce::FontOptions(11.f));
    profileName.setTextToShowWhenEmpty("Optional profile name",juce::Colours::grey);
    preampLabel.setText("SAFETY PREAMP",juce::dontSendNotification);preampLabel.setFont(juce::FontOptions(12.f,juce::Font::bold));
    configureKnob(preamp," dB");preamp.setRange(-40,0,.1);
    searchToggle.setClickingTogglesState(true);searchToggle.setToggleState(false,juce::dontSendNotification);
    sidebarToggle.setClickingTogglesState(true);sidebarToggle.setToggleState(true,juce::dontSendNotification);
    bandsToggle.setClickingTogglesState(true);bandsToggle.setToggleState(true,juce::dontSendNotification);
    bypass.setClickingTogglesState(true);setBypassed(processor.state.getRawParameterValue("headphoneEnabled")->load()<.5f);
    configureToggle(searchToggle);configureToggle(sidebarToggle);configureToggle(bandsToggle);configureToggle(bypass);
    bandsViewport.setViewedComponent(&bandsContent,false);bandsViewport.setScrollBarsShown(true,false,false,false);bandsViewport.setScrollBarThickness(13);bandsViewport.setScrollOnDragMode(juce::Viewport::ScrollOnDragMode::never);
    bandsViewport.getVerticalScrollBar().setColour(juce::ScrollBar::thumbColourId,juce::Colours::white.withAlpha(.82f));
    bandsViewport.getVerticalScrollBar().setColour(juce::ScrollBar::trackColourId,juce::Colour(0xff101923));

    searchOnline.onClick=[this]{auto query=search.getText().trim();if(query.length()<2){showError("Enter at least two characters");return;}status.setText("Searching AutoEq...",juce::dontSendNotification);juce::Component::SafePointer<HeadphoneCorrectionPage>safe(this);processor.searchHeadphonesAsync(query,[safe](std::vector<HeadphoneEntry>found,const juce::String&error){if(!safe)return;safe->results=std::move(found);safe->onlineResults.clear();for(size_t i=0;i<safe->results.size();++i)safe->onlineResults.addItem(safe->results[i].displayName(),(int)i+1);if(!safe->results.empty())safe->onlineResults.setSelectedId(1);safe->status.setText(error.isNotEmpty()?error:juce::String(safe->results.size())+" online measurements found",juce::dontSendNotification);});};
    download.onClick=[this]{auto index=onlineResults.getSelectedItemIndex();if(index<0||index>=(int)results.size()){showError("Select an online measurement first");return;}status.setText("Generating the AutoEq profile...",juce::dontSendNotification);juce::Component::SafePointer<HeadphoneCorrectionPage>safe(this);processor.createHeadphoneProfileAsync(results[(size_t)index],profileName.getText().trim(),[safe](const juce::String&error){if(!safe)return;if(error.isNotEmpty())safe->showError(error);safe->refreshProfiles();safe->status.setText(error.isEmpty()?"Profile saved and loaded":"AutoEq request failed",juce::dontSendNotification);});};
    searchToggle.onClick=[this]{searchOpen=searchToggle.getToggleState();resized();};
    sidebarToggle.onClick=[this]{sidebarOpen=sidebarToggle.getToggleState();resized();};
    bandsToggle.onClick=[this]{bandsOpen=bandsToggle.getToggleState();resized();};
    bypass.onClick=[this]{setBypassed(bypass.getToggleState());};
    save.onClick=[this]{auto row=saved.getSelectedRow();if(row<0||row>=(int)profiles.size())return;auto profile=profiles[(size_t)row];profile.name=profileName.getText().trim().isNotEmpty()?profileName.getText().trim():profile.headphone;profile.preampDb=(float)preamp.getValue();juce::String error;if(!processor.saveHeadphoneProfile(profile,profile.id,error))showError(error);refreshProfiles();saved.selectRow(row);};
    remove.onClick=[this]{auto row=saved.getSelectedRow();if(row<0||row>=(int)profiles.size())return;juce::String error;if(!processor.deleteHeadphoneProfile(profiles[(size_t)row].id,error))showError(error);refreshProfiles();};
    preamp.onValueChange=[this]{if(updating)return;auto row=saved.getSelectedRow();if(row>=0&&row<(int)profiles.size()){profiles[(size_t)row].preampDb=(float)preamp.getValue();curve.repaint();}};

    auto addComponent=[this](juce::Component&component){addAndMakeVisible(component);};
    addComponent(heading);addComponent(status);addComponent(searchToggle);addComponent(search);addComponent(searchOnline);addComponent(onlineResults);addComponent(profileNameHint);addComponent(profileName);addComponent(download);
    addComponent(sidebarToggle);addComponent(saved);addComponent(bypass);addComponent(details);addComponent(save);addComponent(remove);addComponent(curve);addComponent(bandsToggle);addComponent(bandsViewport);
    setContextHelp(searchToggle,"Show or hide the AutoEQ search.");setContextHelp(search,"Enter a headphone model.");setContextHelp(searchOnline,"Search AutoEQ online.");setContextHelp(onlineResults,"Choose an online measurement.");setContextHelp(profileName,"Set an optional local profile name.");setContextHelp(download,"Save and load this AutoEQ profile.");
    setContextHelp(sidebarToggle,"Show or hide saved profiles.");setContextHelp(saved,"Select a profile; double-click to load.");setContextHelp(bypass,"Enable or bypass headphone correction.");setContextHelp(save,"Save profile and EQ changes.");setContextHelp(remove,"Delete the selected local profile.");setContextHelp(curve,"Shows the total headphone EQ response.");setContextHelp(bandsToggle,"Show or hide editable EQ bands.");setContextHelp(preamp,"Reduce level before headphone EQ.");
    refreshProfiles();startTimerHz(4);
}
void HeadphoneCorrectionPage::setBypassed(bool value){bypass.setToggleState(value,juce::dontSendNotification);bypass.setButtonText(value?"BYPASS ON":"BYPASS OFF");if(auto*parameter=processor.state.getParameter("headphoneEnabled"))parameter->setValueNotifyingHost(value?0.f:1.f);}
void HeadphoneCorrectionPage::refreshProfiles(){auto selected=saved.getSelectedRow();profiles=processor.headphoneProfiles();saved.updateContent();if(!profiles.empty()){auto row=juce::jlimit(0,(int)profiles.size()-1,selected<0?0:selected);saved.selectRow(row);showProfile(row);}else showProfile(-1);repaint();}
void HeadphoneCorrectionPage::paintListBoxItem(int row,juce::Graphics&g,int width,int height,bool selected){if(row<0||row>=(int)profiles.size())return;if(selected)g.fillAll(juce::Colour(0xff315b78));auto active=profiles[(size_t)row].id==processor.activeHeadphoneProfileId();g.setColour(juce::Colours::white);g.drawText((active?"> ":"")+profiles[(size_t)row].name+"  —  "+profiles[(size_t)row].headphone,8,0,width-16,height,juce::Justification::centredLeft,true);}
void HeadphoneCorrectionPage::selectedRowsChanged(int row){showProfile(row);}
void HeadphoneCorrectionPage::listBoxItemDoubleClicked(int row,const juce::MouseEvent&){if(row<0||row>=(int)profiles.size())return;processor.activateHeadphoneProfile(profiles[(size_t)row].id);status.setText("Loaded "+profiles[(size_t)row].name,juce::dontSendNotification);saved.repaint();}
void HeadphoneCorrectionPage::showProfile(int row){updating=true;if(row<0||row>=(int)profiles.size()){profileName.clear();details.setText("No saved profile",juce::dontSendNotification);preamp.setValue(0,juce::dontSendNotification);curve.setProfile(nullptr,processor.getSampleRate());updating=false;rebuildBands();return;}auto&p=profiles[(size_t)row];profileName.setText(p.name==p.headphone?juce::String{}:p.name,false);preamp.setValue(p.preampDb,juce::dontSendNotification);details.setText(p.headphone+" | "+p.source+" | "+p.rig+" | "+p.target,juce::dontSendNotification);curve.setProfile(&p,processor.getSampleRate());updating=false;rebuildBands();}
void HeadphoneCorrectionPage::rebuildBands(){bandsContent.removeAllChildren();bands.clear();bandsContent.addAndMakeVisible(preampLabel);bandsContent.addAndMakeVisible(preamp);auto row=saved.getSelectedRow();if(row>=0&&row<(int)profiles.size())for(size_t i=0;i<profiles[(size_t)row].filters.size();++i){auto editor=std::make_unique<BandEditor>(profiles[(size_t)row].filters[i],(int)i,[this]{curve.repaint();});bandsContent.addAndMakeVisible(*editor);bands.push_back(std::move(editor));}const auto columns=getWidth()>1050?3:2;const auto cellWidth=juce::jmax(240,bandsViewport.getWidth()/columns);const auto cells=(int)bands.size()+1;const auto rows=juce::jmax(1,(cells+columns-1)/columns);bandsContent.setSize(juce::jmax(bandsViewport.getWidth()-14,cellWidth*columns),rows*178);auto preampCell=juce::Rectangle<int>(0,0,cellWidth-6,172).reduced(8);preampLabel.setBounds(preampCell.removeFromTop(28));preamp.setBounds(preampCell);for(size_t i=0;i<bands.size();++i){const auto cell=(int)i+1;bands[i]->setBounds(cell%columns*cellWidth,cell/columns*178,cellWidth-6,172);}}
void HeadphoneCorrectionPage::showError(const juce::String&message){juce::NativeMessageBox::showMessageBoxAsync(juce::MessageBoxIconType::WarningIcon,"ReferenceLab Headphones",message);}
void HeadphoneCorrectionPage::timerCallback(){saved.repaint();const auto shouldBypass=processor.state.getRawParameterValue("headphoneEnabled")->load()<.5f;if(bypass.getToggleState()!=shouldBypass){bypass.setToggleState(shouldBypass,juce::dontSendNotification);bypass.setButtonText(shouldBypass?"BYPASS ON":"BYPASS OFF");}}
void HeadphoneCorrectionPage::paint(juce::Graphics&g){g.fillAll(juce::Colour(0xff151d27));auto area=getLocalBounds().toFloat().reduced(8);g.setColour(juce::Colour(0xff16212c));g.fillRoundedRectangle(area,8);g.setColour(juce::Colour(0xff304356));g.drawRoundedRectangle(area,8,1);}
void HeadphoneCorrectionPage::resized(){
    auto area=getLocalBounds().reduced(16);heading.setBounds(area.removeFromTop(30));status.setBounds(area.removeFromTop(24));searchToggle.setBounds(area.removeFromTop(38).removeFromLeft(138));area.removeFromTop(5);
    search.setVisible(searchOpen);searchOnline.setVisible(searchOpen);onlineResults.setVisible(searchOpen);profileNameHint.setVisible(searchOpen);profileName.setVisible(searchOpen);download.setVisible(searchOpen);
    if(searchOpen){auto searchRow=area.removeFromTop(38);searchOnline.setBounds(searchRow.removeFromRight(150));searchRow.removeFromRight(6);search.setBounds(searchRow);area.removeFromTop(5);onlineResults.setBounds(area.removeFromTop(36));area.removeFromTop(5);profileNameHint.setBounds(area.removeFromTop(20));auto saveRow=area.removeFromTop(38);download.setBounds(saveRow.removeFromRight(180));saveRow.removeFromRight(6);profileName.setBounds(saveRow);area.removeFromTop(10);}
    auto toolbar=area.removeFromTop(40);sidebarToggle.setBounds(toolbar.removeFromLeft(104));toolbar.removeFromLeft(6);bypass.setBounds(toolbar.removeFromLeft(112));toolbar.removeFromLeft(6);save.setBounds(toolbar.removeFromLeft(130));toolbar.removeFromLeft(6);remove.setBounds(toolbar.removeFromLeft(130));toolbar.removeFromLeft(6);bandsToggle.setBounds(toolbar.removeFromLeft(110));area.removeFromTop(8);
    saved.setVisible(sidebarOpen);auto content=area;if(sidebarOpen){auto side=content.removeFromLeft(juce::jlimit(240,360,getWidth()/3));saved.setBounds(side);content.removeFromLeft(10);}
    details.setBounds(content.removeFromTop(30));curve.setBounds(content.removeFromTop(juce::jlimit(128,190,content.getHeight()/3)));content.removeFromTop(6);
    bandsViewport.setVisible(bandsOpen);if(bandsOpen)bandsViewport.setBounds(content);rebuildBands();
}
}
