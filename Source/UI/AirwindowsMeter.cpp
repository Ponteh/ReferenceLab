#include "AirwindowsMeter.h"
#include <cmath>

namespace referencelab {
namespace {
const auto panel=juce::Colour(0xff121b25);
const auto plot=juce::Colour(0xff0b1118);
const auto border=juce::Colour(0xff304356);
const auto text=juce::Colour(0xfff1f4f7);
const auto secondary=juce::Colour(0xffaebdca);
const auto grid=juce::Colour(0xff263544);

float clamp01(float value){return juce::jlimit(0.0f,1.0f,value);}
float loudnessY(float value,const juce::Rectangle<float>&area){return area.getBottom()-clamp01(value)*area.getHeight();}
float slewY(float value,const juce::Rectangle<float>&area){return area.getBottom()-clamp01(std::sqrt(juce::jmax(0.0f,value))*1.5f)*area.getHeight();}
float zeroY(float value,const juce::Rectangle<float>&area)
{
    const auto meterZero=std::sqrt(juce::jmax(0.0f,value))*6.0f-6.0f;
    const auto bass=juce::jlimit(-20.0f,199.0f,std::sqrt(juce::jmax(0.0f,meterZero))*16.2f-20.0f);
    return area.getY()+((bass+20.0f)/219.0f)*area.getHeight();
}
juce::Colour activityColour(float peakValue,float slewValue)
{
    const auto peak=peakValue*200.0f;
    const auto slew=std::sqrt(juce::jmax(0.0f,slewValue))*300.0f;
    if(peak>196.0f)return juce::Colour(0xffff5252);
    if(slew>peak*1.15f)return juce::Colour(0xffff9f43);
    if(peak>slew)return juce::Colour(0xff55d68b);
    return juce::Colour(0xff59bfff);
}
void drawLabel(juce::Graphics&g,const juce::String&s,juce::Rectangle<float>area,float size,juce::Justification justification,juce::Colour colour=text)
{
    g.setColour(colour);g.setFont(juce::Font(juce::FontOptions(size)));g.drawText(s,area.toNearestInt(),justification,false);
}
}

AirwindowsMeter::AirwindowsMeter(AirwindowsMeterModel&m):model(m)
{
    setOpaque(true);setMouseCursor(juce::MouseCursor::PointingHandCursor);
    setTooltip("Meter temporale RMS, peak, slew e zero crossing. Doppio clic per cancellare la vista.");
    startTimerHz(30);
}

void AirwindowsMeter::setSourceColours(juce::Colour mix,juce::Colour reference)
{
    mixColour=mix;referenceColour=reference;repaint();
}

void AirwindowsMeter::append(const AirwindowsMeterFrame&frame)noexcept
{
    history[(size_t)writePosition]=frame;
    writePosition=(writePosition+1)%historyCapacity;
    historyCount=juce::jmin(historyCount+1,historyCapacity);
}

void AirwindowsMeter::timerCallback()
{
    AirwindowsMeterFrame incoming[64];
    const auto count=model.pull(incoming,64);
    for(int i=0;i<count;++i)append(incoming[i]);
    if(count>0)repaint();
}

void AirwindowsMeter::mouseDoubleClick(const juce::MouseEvent&)
{
    historyCount=0;writePosition=0;repaint();
}

void AirwindowsMeter::paint(juce::Graphics&g)
{
    g.fillAll(juce::Colour(0xff10151d));
    auto content=getLocalBounds().toFloat().reduced(12.0f);
    auto header=content.removeFromTop(48.0f);
    auto footer=content.removeFromBottom(24.0f);
    content.removeFromTop(4.0f);

    drawLabel(g,"AIRWINDOWS METER",header.removeFromLeft(190.0f),17.0f,juce::Justification::centredLeft);
    drawLabel(g,"RMS / PEAK  •  SLEW  •  ZERO CROSS",header,11.0f,juce::Justification::centredLeft,secondary);

    const auto sourceLegend=[&](juce::Rectangle<float>area,juce::Colour colour,const juce::String&label){
        g.setColour(colour);g.fillRoundedRectangle(area.removeFromLeft(18.0f).reduced(2.0f,7.0f),2.0f);
        drawLabel(g,label,area,10.0f,juce::Justification::centredLeft,secondary);
    };
    auto clearHint=footer.removeFromRight(150.0f);
    auto legend=footer;
    sourceLegend(legend.removeFromLeft(116.0f),mixColour,"MIX HISTORY");
    sourceLegend(legend.removeFromLeft(152.0f),referenceColour,"REFERENCE HISTORY");
    g.setColour(text.withAlpha(0.85f));g.fillRect(legend.removeFromLeft(18.0f).withSizeKeepingCentre(1.0f,14.0f));
    drawLabel(g,"SOURCE CHANGE",legend,10.0f,juce::Justification::centredLeft,secondary);
    drawLabel(g,"DOUBLE-CLICK: CLEAR",clearHint,10.0f,juce::Justification::centredRight,secondary);

    constexpr float laneGap=8.0f;
    const auto laneHeight=(content.getHeight()-laneGap*2.0f)/3.0f;
    std::array<juce::Rectangle<float>,3>lanes;
    for(int i=0;i<3;++i){lanes[(size_t)i]=content.removeFromTop(laneHeight);if(i<2)content.removeFromTop(laneGap);}
    const auto railWidth=juce::jlimit(88.0f,124.0f,(float)getWidth()*0.145f);
    const juce::String titles[]{"LOUDNESS / PEAK  ·  LEFT + RIGHT","DETAIL / SLEW  ·  LEFT + RIGHT","FULLNESS / ZERO CROSS  ·  LEFT + RIGHT"};
    const std::array<juce::StringArray,3>scaleLabels{{
        {"0","-6","-12","-18","-24","-36 dB"},
        {"100","80","60","40","20","0 %"},
        {"900","210","100","60","45","30 Hz"}}};
    const std::array<std::array<float,6>,3>scalePositions{{
        {{0.03f,0.30f,0.505f,0.65f,0.751f,0.876f}},
        {{0.03f,0.20f,0.40f,0.60f,0.80f,0.97f}},
        {{0.03f,0.30f,0.505f,0.65f,0.751f,0.913f}}}};

    std::array<juce::Rectangle<float>,3>plots;
    for(int lane=0;lane<3;++lane)
    {
        auto whole=lanes[(size_t)lane];
        g.setColour(panel);g.fillRoundedRectangle(whole,7.0f);g.setColour(border);g.drawRoundedRectangle(whole,7.0f,1.0f);
        auto laneTitle=whole.removeFromTop(24.0f);
        drawLabel(g,titles[lane],laneTitle.reduced(10.0f,0.0f),10.5f,juce::Justification::centredLeft);
        auto rail=whole.removeFromLeft(railWidth);
        auto graph=whole.reduced(5.0f,5.0f);plots[(size_t)lane]=graph;
        g.setColour(plot);g.fillRoundedRectangle(graph,4.0f);
        auto scaleArea=rail.reduced(10.0f,4.0f);
        for(int tick=0;tick<6;++tick)
        {
            const auto y=graph.getY()+scalePositions[(size_t)lane][(size_t)tick]*graph.getHeight();
            drawLabel(g,scaleLabels[(size_t)lane][tick],{scaleArea.getX(),y-7.0f,scaleArea.getWidth(),14.0f},9.5f,juce::Justification::centredRight,secondary);
            g.setColour(grid);g.drawHorizontalLine((int)std::round(y),graph.getX(),graph.getRight());
        }
    }

    const auto plotWidth=juce::jmax(1,(int)std::floor(plots[0].getWidth()));
    const auto visible=juce::jmin(historyCount,plotWidth);
    if(visible==0)
    {
        for(auto graph:plots)drawLabel(g,"WAITING FOR AUDIO",graph,11.0f,juce::Justification::centred,secondary.withAlpha(0.7f));
        return;
    }

    const auto first=(writePosition-visible+historyCapacity)%historyCapacity;
    const auto step=plots[0].getWidth()/(float)juce::jmax(1,visible);
    for(int i=0;i<visible;++i)
    {
        const auto&frame=history[(size_t)((first+i)%historyCapacity)];
        const auto source=frame.reference?referenceColour:mixColour;
        for(auto graph:plots)
        {
            const auto x=graph.getX()+i*step;
            g.setColour(source.withAlpha(0.95f));g.fillRect(x,graph.getBottom()-3.0f,juce::jmax(1.0f,step+0.4f),3.0f);
            if(frame.boundary&&i>0){g.setColour(text.withAlpha(0.9f));g.fillRect(x,graph.getY()+2.0f,1.0f,graph.getHeight()-6.0f);}
        }

        const auto x=plots[0].getX()+i*step+step*0.5f;
        const auto point=[&](int lane,float y,juce::Colour colour,float size,float xOffset){
            const auto graph=plots[(size_t)lane];
            g.setColour(colour.withAlpha(0.78f));g.fillEllipse(x+xOffset-size*0.5f,y-size*0.5f,size,size);
        };
        const auto rmsSizeL=juce::jlimit(1.0f,5.0f,1.0f+frame.rmsL*4.0f);
        const auto rmsSizeR=juce::jlimit(1.0f,5.0f,1.0f+frame.rmsR*4.0f);
        point(0,loudnessY(frame.peakL,plots[0]),activityColour(frame.peakL,frame.slewL),rmsSizeL,-1.0f);
        point(0,loudnessY(frame.peakR,plots[0]),activityColour(frame.peakR,frame.slewR),rmsSizeR,1.0f);
        point(1,slewY(frame.slewL,plots[1]),activityColour(frame.peakL,frame.slewL),2.2f,-1.0f);
        point(1,slewY(frame.slewR,plots[1]),activityColour(frame.peakR,frame.slewR),2.2f,1.0f);
        point(2,zeroY(frame.zeroL,plots[2]),juce::Colour(0xffbf78ff),2.2f,-1.0f);
        point(2,zeroY(frame.zeroR,plots[2]),juce::Colour(0xff61d8d2),2.2f,1.0f);
    }
}
}
