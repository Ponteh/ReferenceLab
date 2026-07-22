#include "ReferenceWaveform.h"
#include <cmath>
#include <vector>

namespace referencelab {
ReferenceWaveform::ReferenceWaveform(ReferencePlayer&p):player(p){startTimerHz(30);}

double ReferenceWaveform::duration()const noexcept{return player.getDurationSeconds();}
double ReferenceWaveform::xToSeconds(float x)const noexcept{auto d=duration();return d<=0?0:juce::jlimit(0.0,d,viewStart+juce::jlimit(0.f,1.f,x/(float)juce::jmax(1,getWidth()))*viewLength*d);}
float ReferenceWaveform::secondsToX(double seconds)const noexcept{auto d=duration();return d<=0?0:(float)((seconds/d-viewStart)/viewLength*getWidth());}
void ReferenceWaveform::setLoop(bool enabled,double start,double end)noexcept{loopEnabled=enabled;loopStart=start;loopEnd=end;repaint();}

void ReferenceWaveform::paint(juce::Graphics&g){
    auto area=getLocalBounds().toFloat();g.setColour(juce::Colour(0xff111923));g.fillRoundedRectangle(area,5);
    auto audio=player.getAudioSnapshot();if(!audio||audio->samples.getNumSamples()==0){g.setColour(juce::Colours::grey);g.drawText("No waveform",getLocalBounds(),juce::Justification::centred);return;}
    auto d=duration();auto first=(int)(viewStart*audio->samples.getNumSamples());auto count=juce::jmax(1,(int)(viewLength*audio->samples.getNumSamples()));juce::Path path;auto centre=area.getCentreY();std::vector<float>peaks((size_t)juce::jmax(0,getWidth()));
    for(int x=0;x<getWidth();++x){auto a=juce::jlimit(0,audio->samples.getNumSamples()-1,first+(int)((double)x/juce::jmax(1,getWidth())*count));auto b=juce::jlimit(a+1,audio->samples.getNumSamples(),first+(int)((double)(x+1)/juce::jmax(1,getWidth())*count));auto step=juce::jmax(1,(b-a)/8);for(int i=a;i<b;i+=step)peaks[(size_t)x]=juce::jmax(peaks[(size_t)x],std::abs(audio->samples.getSample(0,i)),std::abs(audio->samples.getSample(1,i)));auto height=peaks[(size_t)x]*area.getHeight()*.42f;if(x==0)path.startNewSubPath(0,centre-height);else path.lineTo((float)x,centre-height);}
    for(int x=getWidth()-1;x>=0;--x)path.lineTo((float)x,centre+peaks[(size_t)x]*area.getHeight()*.42f);path.closeSubPath();g.setColour(referenceColour.withAlpha(.75f));g.fillPath(path);
    if(loopEnabled&&loopEnd>loopStart){auto startX=secondsToX(loopStart),endX=secondsToX(loopEnd);auto visibleStart=juce::jlimit(0.f,area.getRight(),startX),visibleEnd=juce::jlimit(0.f,area.getRight(),endX);if(visibleEnd>visibleStart){g.setColour(juce::Colour(0x334bc0ff));g.fillRect(juce::Rectangle<float>(visibleStart,0,visibleEnd-visibleStart,area.getHeight()));}g.setColour(juce::Colour(0xff4bc0ff));if(startX>=0&&startX<=area.getRight()){g.drawVerticalLine((int)startX,0,area.getHeight());g.drawText("START",(int)startX+4,2,42,18,juce::Justification::left);}if(endX>=0&&endX<=area.getRight()){g.drawVerticalLine((int)endX,0,area.getHeight());g.drawText("END",(int)endX-45,2,42,18,juce::Justification::right);}}
    auto rawCurrent=secondsToX(player.getPositionSeconds());auto current=juce::jlimit(0.f,area.getRight(),rawCurrent);g.setColour(juce::Colours::white);g.drawVerticalLine((int)current,0,area.getHeight());auto currentLabel=rawCurrent<0?"< CURRENT":rawCurrent>area.getRight()?"CURRENT >":"CURRENT";g.drawText(currentLabel,juce::jlimit(2,(int)area.getRight()-68,(int)current-34),20,68,16,juce::Justification::centred);
    g.setFont(11.f);g.drawText(juce::String(player.getPositionSeconds(),1)+" / "+juce::String(d,1)+" s",6,getHeight()-22,150,18,juce::Justification::left);if(loopEnabled&&loopEnd>loopStart)g.drawText("Loop "+juce::String(loopEnd-loopStart,1)+" s",getWidth()/2-55,getHeight()-22,110,18,juce::Justification::centred);g.drawText("Zoom "+juce::String(1.0/viewLength,1)+"x",getWidth()-92,getHeight()-22,86,18,juce::Justification::right);
}

void ReferenceWaveform::seekFromMouse(float x){const auto seconds=juce::jlimit(0.0,duration(),xToSeconds(x)+dragSeekOffset);player.seek(seconds);if(onSeek)onSeek(seconds);}
void ReferenceWaveform::mouseDown(const juce::MouseEvent&e){dragStartX=e.x;dragViewStart=viewStart;dragSeekOffset=0;if(e.mods.isRightButtonDown()||e.mods.isMiddleButtonDown())drag=Drag::pan;else if(loopEnabled&&std::abs(e.x-secondsToX(loopStart))<8)drag=Drag::start;else if(loopEnabled&&std::abs(e.x-secondsToX(loopEnd))<8)drag=Drag::end;else{drag=Drag::seek;const auto currentX=secondsToX(player.getPositionSeconds());if(currentX>=0&&currentX<=getWidth()&&std::abs(e.position.x-currentX)<=34.f)dragSeekOffset=player.getPositionSeconds()-xToSeconds(e.position.x);else seekFromMouse(e.position.x);}}
void ReferenceWaveform::mouseDrag(const juce::MouseEvent&e){if(drag==Drag::pan)viewStart=juce::jlimit(0.0,1.0-viewLength,dragViewStart-(e.x-dragStartX)/(double)juce::jmax(1,getWidth())*viewLength);else if(drag==Drag::start)loopStart=juce::jlimit(0.0,loopEnd-.01,xToSeconds((float)e.x));else if(drag==Drag::end)loopEnd=juce::jlimit(loopStart+.01,duration(),xToSeconds((float)e.x));else seekFromMouse(e.position.x);repaint();}
void ReferenceWaveform::mouseUp(const juce::MouseEvent&){if((drag==Drag::start||drag==Drag::end)&&onLoopChanged)onLoopChanged(loopStart,loopEnd);}
void ReferenceWaveform::mouseWheelMove(const juce::MouseEvent&e,const juce::MouseWheelDetails&wheel){if(std::abs(wheel.deltaY)<.001f)return;auto old=viewLength;viewLength=juce::jlimit(.05,1.0,viewLength*(wheel.deltaY>0?.75:1.333));auto anchor=viewStart+old*e.position.x/juce::jmax(1,getWidth());viewStart=juce::jlimit(0.0,1.0-viewLength,anchor-viewLength*e.position.x/juce::jmax(1,getWidth()));repaint();}
void ReferenceWaveform::timerCallback(){repaint();}
}
