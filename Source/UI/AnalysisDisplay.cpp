#include "AnalysisDisplay.h"
#include "FrequencyPlot.h"
#include "UiTheme.h"
#include <algorithm>
namespace referencelab {
AnalysisDisplay::AnalysisDisplay(SampleFifo&m,SampleFifo&r,SampleFifo&o,juce::AudioProcessorValueTreeState&s):mixFifo(m),referenceFifo(r),outputFifo(o),state(s){setMouseCursor(juce::MouseCursor::CrosshairCursor);startTimerHz(30);}
void AnalysisDisplay::setFftOrder(int order){mixAnalyzer.setFftOrder(order);referenceAnalyzer.setFftOrder(order);}
void AnalysisDisplay::consume(SampleFifo&fifo,std::array<float,maxSize>&target){const int count=fifo.pull(scratch.data(),maxSize);if(count<=0)return;if(count>=maxSize)std::copy_n(scratch.end()-maxSize,maxSize,target.begin());else{std::move(target.begin()+count,target.end(),target.begin());std::copy_n(scratch.begin(),count,target.end()-count);}}
void AnalysisDisplay::timerCallback(){if(frozen)return;mixAnalyzer.update(mixFifo);referenceAnalyzer.update(referenceFifo);consume(outputFifo,outputTime);repaint();}
void AnalysisDisplay::mouseMove(const juce::MouseEvent&e){cursorPosition=e.position;cursorVisible=true;repaint();}
void AnalysisDisplay::mouseDrag(const juce::MouseEvent&e){mouseMove(e);}
void AnalysisDisplay::mouseExit(const juce::MouseEvent&){cursorVisible=false;repaint();}
void AnalysisDisplay::paintCursor(juce::Graphics&g){if(!cursorVisible)return;auto all=getLocalBounds().toFloat();auto spectrum=all.removeFromTop(all.getHeight()*.68f).reduced(8);if(!spectrum.contains(cursorPosition))return;auto proportion=juce::jlimit(0.f,1.f,(cursorPosition.x-spectrum.getX())/spectrum.getWidth());auto frequency=FrequencyPlot::frequencyAt(proportion);auto db=juce::jmap(cursorPosition.y,spectrum.getBottom(),spectrum.getY(),-90.f,0.f);g.setColour(juce::Colours::white.withAlpha(.55f));g.drawVerticalLine((int)cursorPosition.x,spectrum.getY(),spectrum.getBottom());g.drawHorizontalLine((int)cursorPosition.y,spectrum.getX(),spectrum.getRight());g.setColour(juce::Colours::white);g.fillEllipse(cursorPosition.x-3.f,cursorPosition.y-3.f,6.f,6.f);auto frequencyText=frequency>=1000.f?juce::String(frequency/1000.f,2)+" kHz":juce::String(juce::roundToInt(frequency))+" Hz";auto text=frequencyText+"   "+juce::String(db,1)+" dB";juce::Rectangle<int>label(0,0,132,22);label.setPosition((int)juce::jlimit(spectrum.getX(),spectrum.getRight()-label.getWidth(),cursorPosition.x+8.f),(int)juce::jlimit(spectrum.getY(),spectrum.getBottom()-label.getHeight(),cursorPosition.y-27.f));g.setColour(juce::Colour(0xee10151d));g.fillRoundedRectangle(label.toFloat(),4.f);g.setColour(juce::Colours::white);g.setFont(11.f);g.drawText(text,label.reduced(6,0),juce::Justification::centredLeft);}
void AnalysisDisplay::paint(juce::Graphics&g){
    auto all=getLocalBounds().toFloat();g.fillAll(juce::Colour(0xff151d27));
    auto spectrum=all.removeFromTop(all.getHeight()*.68f).reduced(8);g.setFont(10);
    for(auto db:{0,-18,-36,-54,-72,-90}){auto y=juce::jmap((float)db,-90.f,0.f,spectrum.getBottom(),spectrum.getY());g.setColour(juce::Colours::white.withAlpha(.08f));g.drawHorizontalLine((int)y,spectrum.getX(),spectrum.getRight());g.setColour(juce::Colours::white.withAlpha(.45f));g.drawText(juce::String(db)+" dB",(int)spectrum.getX()+2,(int)y-12,42,12,juce::Justification::left);}
    for(auto f:{20,50,100,200,500,1000,2000,5000,10000,20000}){auto x=FrequencyPlot::xForFrequency((float)f,spectrum);g.setColour(juce::Colours::white.withAlpha(.07f));g.drawVerticalLine((int)x,spectrum.getY(),spectrum.getBottom());g.setColour(juce::Colours::white.withAlpha(.45f));g.drawText(f>=1000?juce::String(f/1000)+"k":juce::String(f),(int)x-14,(int)spectrum.getBottom()-14,28,12,juce::Justification::centred);}
    auto mixColour=UiTheme::mix(state.state),referenceColour=UiTheme::reference(state.state),eqColour=UiTheme::equalizer();
    g.setColour(mixColour);g.strokePath(FrequencyPlot::spectrumPath(mixAnalyzer.getSpectrum(),mixAnalyzer.getFftSize(),sampleRate,spectrum),juce::PathStrokeType(1.5f));
    g.setColour(referenceColour);g.strokePath(FrequencyPlot::spectrumPath(referenceAnalyzer.getSpectrum(),referenceAnalyzer.getFftSize(),sampleRate,spectrum),juce::PathStrokeType(1.5f));
    g.setColour(eqColour);g.strokePath(FrequencyPlot::equalizerPath(state,sampleRate,spectrum),juce::PathStrokeType(1.8f));
    auto legend=spectrum.toNearestInt().removeFromTop(20);g.setColour(mixColour);g.drawText("MIX",legend.removeFromLeft(35),juce::Justification::left);g.setColour(referenceColour);g.drawText("REFERENCE",legend.removeFromLeft(72),juce::Justification::left);g.setColour(eqColour);g.drawText("EQ",legend.removeFromLeft(30),juce::Justification::left);
    auto scope=all.reduced(8);juce::Path wave;for(int x=0;x<(int)scope.getWidth();++x){auto index=juce::jlimit(0,maxSize-1,(int)(x/juce::jmax(1.f,scope.getWidth())*maxSize));auto y=scope.getCentreY()-outputTime[(size_t)index]*scope.getHeight()*.45f;if(x==0)wave.startNewSubPath(scope.getX(),y);else wave.lineTo(scope.getX()+x,y);}g.setColour(eqColour);g.strokePath(wave,juce::PathStrokeType(1));g.setColour(juce::Colours::white.withAlpha(.65f));g.drawText("OUTPUT SCOPE",scope.toNearestInt().removeFromTop(20),juce::Justification::left);paintCursor(g);
}
}
