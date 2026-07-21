#include "AnalysisMeters.h"

namespace referencelab {
void AnalysisMeters::setSnapshots(MeterSnapshot mix, MeterSnapshot reference, MeterSnapshot output)
{
    mixSnapshot=mix;referenceSnapshot=reference;outputSnapshot=output;repaint();
}

void AnalysisMeters::setSourceColours(juce::Colour mix,juce::Colour reference) noexcept
{
    if(mixColour==mix&&referenceColour==reference)return;mixColour=mix;referenceColour=reference;repaint();
}

void AnalysisMeters::drawLevelBar(juce::Graphics&g,juce::Rectangle<float>area,float db,juce::Colour colour,const juce::String&name)
{
    g.setColour(juce::Colours::black.withAlpha(.4f));g.fillRoundedRectangle(area,2.f);
    auto amount=juce::jlimit(0.f,1.f,(db+60.f)/60.f);auto fill=area.withWidth(area.getWidth()*amount);
    g.setColour(db>=-1.f?juce::Colour(0xffff5d5d):colour);g.fillRoundedRectangle(fill,2.f);
    g.setColour(juce::Colours::white.withAlpha(.88f));g.setFont(10.f);g.drawText(name+" "+juce::String(db,1)+" dB",area.toNearestInt().reduced(4,0),juce::Justification::centredLeft);
}

void AnalysisMeters::drawSource(juce::Graphics&g,juce::Rectangle<float>area,const juce::String&title,const MeterSnapshot&m,juce::Colour colour)
{
    g.setColour(juce::Colour(0xff151d27));g.fillRoundedRectangle(area,5.f);g.setColour(colour);g.drawRoundedRectangle(area,5.f,1.2f);area.reduce(8.f,5.f);
    g.setFont(juce::FontOptions(12.f,juce::Font::bold));g.drawText(title,area.removeFromTop(18.f).toNearestInt(),juce::Justification::centredLeft);
    drawLevelBar(g,area.removeFromTop(17.f).reduced(0,2),m.peakDb,colour,"PEAK");drawLevelBar(g,area.removeFromTop(17.f).reduced(0,2),m.rmsDb,colour.withAlpha(.7f),"RMS");
    g.setFont(9.5f);g.setColour(juce::Colours::white.withAlpha(.86f));
    g.drawText("LUFS I "+juce::String(m.integratedLufs,1)+"   S "+juce::String(m.shortTermLufs,1)+"   M "+juce::String(m.momentaryLufs,1),area.removeFromTop(17.f).toNearestInt(),juce::Justification::centredLeft);
    g.drawText("TP "+juce::String(m.truePeakDbtp,1)+" dBTP   Crest "+juce::String(m.crestDb,1)+" dB",area.removeFromTop(17.f).toNearestInt(),juce::Justification::centredLeft);
    g.drawText("Dynamic Range  "+juce::String(m.dynamicRangeDb,1)+" dB",area.removeFromTop(17.f).toNearestInt(),juce::Justification::centredLeft);
}

void AnalysisMeters::drawOutput(juce::Graphics&g,juce::Rectangle<float>area,const MeterSnapshot&m)
{
    auto colour=juce::Colour(0xff7f8a94);g.setColour(juce::Colour(0xff151d27));g.fillRoundedRectangle(area,5.f);g.setColour(colour);g.drawRoundedRectangle(area,5.f,1.2f);area.reduce(8.f,5.f);
    g.setColour(juce::Colours::white);g.setFont(juce::FontOptions(12.f,juce::Font::bold));g.drawText("OUTPUT",area.removeFromTop(18.f).toNearestInt(),juce::Justification::centredLeft);
    drawLevelBar(g,area.removeFromTop(17.f).reduced(0,2),m.peakDb,colour,"PEAK");drawLevelBar(g,area.removeFromTop(17.f).reduced(0,2),m.rmsDb,colour.withAlpha(.7f),"RMS");
    auto correlationArea=area.removeFromTop(17.f).reduced(0,2);g.setColour(juce::Colours::black.withAlpha(.4f));g.fillRoundedRectangle(correlationArea,2.f);auto centre=correlationArea.getCentreX();auto end=juce::jmap(juce::jlimit(-1.f,1.f,m.correlation),-1.f,1.f,correlationArea.getX(),correlationArea.getRight());g.setColour(m.correlation<0.f?juce::Colour(0xffff5d5d):colour);g.fillRect(juce::Rectangle<float>::leftTopRightBottom(juce::jmin(centre,end),correlationArea.getY(),juce::jmax(centre,end),correlationArea.getBottom()));g.setColour(juce::Colours::white.withAlpha(.88f));g.setFont(9.5f);g.drawText("CORR "+juce::String(m.correlation,2),correlationArea.toNearestInt().reduced(4,0),juce::Justification::centredLeft);
    auto widthArea=area.removeFromTop(17.f).reduced(0,2);g.setColour(juce::Colours::black.withAlpha(.4f));g.fillRoundedRectangle(widthArea,2.f);g.setColour(colour.withAlpha(.75f));g.fillRoundedRectangle(widthArea.withWidth(widthArea.getWidth()*juce::jlimit(0.f,1.f,m.stereoWidth)),2.f);g.setColour(juce::Colours::white.withAlpha(.88f));g.drawText("WIDTH "+juce::String(m.stereoWidth,2),widthArea.toNearestInt().reduced(4,0),juce::Justification::centredLeft);
    g.drawText("TP "+juce::String(m.truePeakDbtp,1)+" dBTP   Crest "+juce::String(m.crestDb,1)+" dB",area.removeFromTop(17.f).toNearestInt(),juce::Justification::centredLeft);
}

void AnalysisMeters::paint(juce::Graphics&g)
{
    auto area=getLocalBounds().toFloat();auto gap=6.f;auto width=(area.getWidth()-gap*2.f)/3.f;drawSource(g,area.removeFromLeft(width),"MIX",mixSnapshot,mixColour);area.removeFromLeft(gap);drawSource(g,area.removeFromLeft(width),"REFERENCE",referenceSnapshot,referenceColour);area.removeFromLeft(gap);drawOutput(g,area,outputSnapshot);
}
}
