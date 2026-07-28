#pragma once
#include "../Headphones/HeadphoneProfile.h"
#include <juce_gui_extra/juce_gui_extra.h>
#include <juce_audio_processors/juce_audio_processors.h>

class ReferenceLabAudioProcessor;

namespace referencelab {
class HeadphoneCorrectionPage final:public juce::Component,private juce::ListBoxModel,private juce::Timer {
public:
    explicit HeadphoneCorrectionPage(ReferenceLabAudioProcessor&);
    void paint(juce::Graphics&)override;void resized()override;
private:
    int getNumRows()override{return(int)profiles.size();}
    void paintListBoxItem(int,juce::Graphics&,int,int,bool)override;
    void selectedRowsChanged(int)override;void listBoxItemDoubleClicked(int,const juce::MouseEvent&)override;void timerCallback()override;
    class BandEditor final:public juce::Component {
    public:
        BandEditor(HeadphoneFilter&,int,std::function<void()>);
        void resized()override;void paint(juce::Graphics&)override;
    private:
        HeadphoneFilter&filter;std::function<void()>onEdited;juce::Label title;juce::ComboBox type;juce::Slider frequency,gain,q;
        void capture();
    };
    class CurveDisplay final:public juce::Component {
    public:
        void setProfile(const HeadphoneProfile*value,double sr){profile=value;sampleRate=sr>0?sr:44100.0;repaint();}
        void paint(juce::Graphics&)override;
    private:
        const HeadphoneProfile*profile=nullptr;double sampleRate=44100.0;
    };
    class ScrollbarOnlyViewport final:public juce::Viewport {
    public:void mouseWheelMove(const juce::MouseEvent&,const juce::MouseWheelDetails&)override{}
    };
    void refreshProfiles();void showProfile(int);void showError(const juce::String&);void rebuildBands();void setBypassed(bool);
    ReferenceLabAudioProcessor&processor;
    std::vector<HeadphoneProfile>profiles;std::vector<HeadphoneEntry>results;std::vector<std::unique_ptr<BandEditor>>bands;bool updating=false,searchOpen=false,sidebarOpen=true,bandsOpen=true;
    juce::Label heading,status,details,profileNameHint,preampLabel;juce::TextEditor search,profileName;juce::TextButton searchToggle{"AUTOEQ SEARCH"},searchOnline{"SEARCH AUTOEQ"},download{"SAVE ONLINE PROFILE"},sidebarToggle{"PROFILES"},bypass{"BYPASS"},save{"SAVE CHANGES"},remove{"DELETE PROFILE"},bandsToggle{"EQ BANDS"};
    juce::ComboBox onlineResults;juce::Slider preamp;juce::ListBox saved{"Saved Headphone Profiles",this};CurveDisplay curve;ScrollbarOnlyViewport bandsViewport;juce::Component bandsContent;
};
}
