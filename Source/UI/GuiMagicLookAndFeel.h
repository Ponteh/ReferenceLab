#pragma once

#include <juce_gui_basics/juce_gui_basics.h>

namespace referencelab {
class GuiMagicLookAndFeel final:public juce::LookAndFeel_V4 {
public:
    GuiMagicLookAndFeel();
    void drawRotarySlider(juce::Graphics&,int,int,int,int,float,float,float,juce::Slider&)override;
    void drawLinearSlider(juce::Graphics&,int,int,int,int,float,float,float,juce::Slider::SliderStyle,juce::Slider&)override;
    void drawButtonBackground(juce::Graphics&,juce::Button&,const juce::Colour&,bool,bool)override;
    void drawButtonText(juce::Graphics&,juce::TextButton&,bool,bool)override;
    void drawComboBox(juce::Graphics&,int,int,bool,int,int,int,int,juce::ComboBox&)override;
    void positionComboBoxText(juce::ComboBox&,juce::Label&)override;
    int getTabButtonBestWidth(juce::TabBarButton&,int)override;
    void drawTabButton(juce::TabBarButton&,juce::Graphics&,bool,bool)override;
};
}
