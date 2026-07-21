#include "GuiMagicLookAndFeel.h"

namespace referencelab {
GuiMagicLookAndFeel::GuiMagicLookAndFeel()
{
    setColour(juce::PopupMenu::backgroundColourId,juce::Colour(0xff111a23));
    setColour(juce::PopupMenu::textColourId,juce::Colour(0xffe2ebf1));
    setColour(juce::PopupMenu::highlightedBackgroundColourId,juce::Colour(0xff287da0));
    setColour(juce::PopupMenu::highlightedTextColourId,juce::Colours::white);
    setColour(juce::TooltipWindow::backgroundColourId,juce::Colour(0xf21a2632));
    setColour(juce::TooltipWindow::textColourId,juce::Colour(0xffe5eef4));
    setColour(juce::TooltipWindow::outlineColourId,juce::Colour(0xff3d566b));
    setColour(juce::TabbedButtonBar::tabTextColourId,juce::Colours::white.withAlpha(.78f));
    setColour(juce::TabbedButtonBar::frontTextColourId,juce::Colours::black);
    setColour(juce::ScrollBar::thumbColourId,juce::Colours::white.withAlpha(.82f));
    setColour(juce::ScrollBar::trackColourId,juce::Colour(0xff17212b));
    setColour(juce::ScrollBar::backgroundColourId,juce::Colour(0xff101820));
}

int GuiMagicLookAndFeel::getTabButtonBestWidth(juce::TabBarButton&button,int) { return juce::jmax(76,button.getButtonText().length()*8+28); }

void GuiMagicLookAndFeel::drawTabButton(juce::TabBarButton&button,juce::Graphics&g,bool over,bool down)
{
    auto area=button.getActiveArea().toFloat().reduced(2.f,3.f);const auto selected=button.isFrontTab();auto background=selected?juce::Colours::white:juce::Colour(0xff17222d);if(!selected&&over)background=juce::Colour(0xff253748);if(down)background=background.darker(.12f);g.setColour(background);g.fillRoundedRectangle(area,5.f);g.setColour(selected?juce::Colours::white:juce::Colour(0xff3b5063));g.drawRoundedRectangle(area,5.f,1.f);g.setColour(selected?juce::Colours::black:juce::Colours::white.withAlpha(button.isEnabled()?1.f:.4f));g.setFont(juce::FontOptions(13.f,juce::Font::bold));g.drawFittedText(button.getButtonText(),button.getTextArea().reduced(8,2),juce::Justification::centred,1);
}

void GuiMagicLookAndFeel::drawRotarySlider(juce::Graphics&g,int x,int y,int width,int height,float position,float startAngle,float endAngle,juce::Slider&slider)
{
    auto bounds=juce::Rectangle<float>((float)x,(float)y,(float)width,(float)height).reduced(8.f);
    const auto radius=juce::jmax(8.f,juce::jmin(bounds.getWidth(),bounds.getHeight())*.5f-3.f);
    const auto centre=bounds.getCentre();
    const auto angle=startAngle+position*(endAngle-startAngle);
    const auto outline=slider.findColour(juce::Slider::rotarySliderOutlineColourId);
    const auto accent=slider.findColour(juce::Slider::rotarySliderFillColourId);
    juce::Path track,active;
    track.addCentredArc(centre.x,centre.y,radius,radius,0.f,startAngle,endAngle,true);
    active.addCentredArc(centre.x,centre.y,radius,radius,0.f,startAngle,angle,true);
    g.setColour(outline.withAlpha(.9f));g.strokePath(track,juce::PathStrokeType(4.f,juce::PathStrokeType::curved,juce::PathStrokeType::rounded));
    g.setColour(accent);g.strokePath(active,juce::PathStrokeType(4.f,juce::PathStrokeType::curved,juce::PathStrokeType::rounded));
    const auto body=radius-8.f;
    g.setGradientFill(juce::ColourGradient(juce::Colour(0xff34495d),centre.x-body*.45f,centre.y-body*.55f,juce::Colour(0xff101820),centre.x+body,centre.y+body,true));
    g.fillEllipse(centre.x-body,centre.y-body,body*2.f,body*2.f);
    g.setColour(juce::Colour(0xff526a7e));g.drawEllipse(centre.x-body,centre.y-body,body*2.f,body*2.f,1.f);
    juce::Path pointer;pointer.addRoundedRectangle(-1.5f,-body+5.f,3.f,juce::jmax(8.f,body*.42f),1.5f);
    g.setColour(slider.isEnabled()?accent:accent.withAlpha(.35f));g.fillPath(pointer,juce::AffineTransform::rotation(angle).translated(centre.x,centre.y));
    if(slider.isMouseOverOrDragging()){g.setColour(accent.withAlpha(.12f));g.fillEllipse(centre.x-radius-5.f,centre.y-radius-5.f,(radius+5.f)*2.f,(radius+5.f)*2.f);}
}

void GuiMagicLookAndFeel::drawLinearSlider(juce::Graphics&g,int x,int y,int width,int height,float sliderPos,float minSliderPos,float maxSliderPos,juce::Slider::SliderStyle style,juce::Slider&slider)
{
    if(style!=juce::Slider::LinearHorizontal&&style!=juce::Slider::LinearBar)
    {juce::LookAndFeel_V4::drawLinearSlider(g,x,y,width,height,sliderPos,minSliderPos,maxSliderPos,style,slider);return;}
    const auto centreY=(float)y+(float)height*.5f;
    const auto left=(float)x+7.f,right=(float)(x+width)-7.f;
    g.setColour(slider.findColour(juce::Slider::backgroundColourId));g.fillRoundedRectangle(left,centreY-3.f,right-left,6.f,3.f);
    g.setColour(slider.findColour(juce::Slider::trackColourId));g.fillRoundedRectangle(left,centreY-3.f,juce::jmax(0.f,sliderPos-left),6.f,3.f);
    g.setColour(slider.findColour(juce::Slider::thumbColourId));g.fillEllipse(sliderPos-7.f,centreY-7.f,14.f,14.f);
    if(slider.isMouseOverOrDragging()){g.setColour(slider.findColour(juce::Slider::trackColourId).withAlpha(.2f));g.fillEllipse(sliderPos-11.f,centreY-11.f,22.f,22.f);}
}

void GuiMagicLookAndFeel::drawButtonBackground(juce::Graphics&g,juce::Button&button,const juce::Colour&base,bool highlighted,bool down)
{
    auto bounds=button.getLocalBounds().toFloat().reduced(.75f);
    auto colour=button.getToggleState()?button.findColour(juce::TextButton::buttonOnColourId):base;
    if(highlighted)colour=colour.brighter(.12f);if(down)colour=colour.darker(.18f);
    g.setColour(juce::Colour(0x50000000));g.fillRoundedRectangle(bounds.translated(0.f,1.5f),5.f);
    g.setColour(colour);g.fillRoundedRectangle(bounds,5.f);
    g.setColour(button.getToggleState()?colour.brighter(.35f):juce::Colour(0xff42586b));g.drawRoundedRectangle(bounds,5.f,1.f);
}

void GuiMagicLookAndFeel::drawButtonText(juce::Graphics&g,juce::TextButton&button,bool,bool)
{
    g.setFont(juce::FontOptions().withHeight(13.f).withStyle("Bold"));
    g.setColour(button.findColour(button.getToggleState()?juce::TextButton::textColourOnId:juce::TextButton::textColourOffId).withMultipliedAlpha(button.isEnabled()?1.f:.45f));
    g.drawFittedText(button.getButtonText(),button.getLocalBounds().reduced(7,2),juce::Justification::centred,1);
}

void GuiMagicLookAndFeel::drawComboBox(juce::Graphics&g,int width,int height,bool down,int,int,int,int,juce::ComboBox&box)
{
    auto bounds=juce::Rectangle<float>(0.f,0.f,(float)width,(float)height).reduced(.75f);
    g.setColour(box.findColour(juce::ComboBox::backgroundColourId));g.fillRoundedRectangle(bounds,5.f);
    g.setColour(box.findColour(juce::ComboBox::outlineColourId));g.drawRoundedRectangle(bounds,5.f,1.f);
    const auto arrowX=(float)width-17.f,arrowY=(float)height*.5f;
    juce::Path arrow;arrow.startNewSubPath(arrowX-4.f,arrowY-2.f);arrow.lineTo(arrowX,arrowY+2.f);arrow.lineTo(arrowX+4.f,arrowY-2.f);
    g.setColour(box.findColour(juce::ComboBox::arrowColourId).withAlpha(down?.65f:1.f));g.strokePath(arrow,juce::PathStrokeType(2.f,juce::PathStrokeType::curved,juce::PathStrokeType::rounded));
}

void GuiMagicLookAndFeel::positionComboBoxText(juce::ComboBox&box,juce::Label&label)
{
    label.setBounds(8,1,box.getWidth()-30,box.getHeight()-2);label.setFont(juce::FontOptions().withHeight(13.f));
}
}
