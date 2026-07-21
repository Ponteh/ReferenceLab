#include "AnalysisDisplay.h"
#include "FrequencyPlot.h"
#include "UiTheme.h"
#include <algorithm>
namespace referencelab {
AnalysisDisplay::AnalysisDisplay(SampleFifo&m,SampleFifo&r,SampleFifo&ms,SampleFifo&rs,SampleFifo&o,juce::AudioProcessorValueTreeState&s):mixFifo(m),referenceFifo(r),mixSideFifo(ms),referenceSideFifo(rs),outputFifo(o),state(s){setMouseCursor(juce::MouseCursor::CrosshairCursor);startTimerHz(30);}
void AnalysisDisplay::setFftOrder(int order){mixAnalyzer.setFftOrder(order);referenceAnalyzer.setFftOrder(order);mixSideAnalyzer.setFftOrder(order);referenceSideAnalyzer.setFftOrder(order);}
void AnalysisDisplay::consume(SampleFifo&fifo,std::array<float,scopeSize>&target){const int count=fifo.pull(scratch.data(),scopeSize);if(count<=0)return;if(count>=scopeSize)std::copy_n(scratch.end()-scopeSize,scopeSize,target.begin());else{std::move(target.begin()+count,target.end(),target.begin());std::copy_n(scratch.begin(),count,target.end()-count);}}
void AnalysisDisplay::timerCallback(){if(frozen)return;mixAnalyzer.update(mixFifo);referenceAnalyzer.update(referenceFifo);mixSideAnalyzer.update(mixSideFifo);referenceSideAnalyzer.update(referenceSideFifo);consume(outputFifo,outputTime);repaint();}
void AnalysisDisplay::mouseMove(const juce::MouseEvent&e){cursorPosition=e.position;cursorVisible=true;repaint();}
void AnalysisDisplay::mouseDrag(const juce::MouseEvent&e){mouseMove(e);}
void AnalysisDisplay::mouseExit(const juce::MouseEvent&){cursorVisible=false;repaint();}
juce::Rectangle<float> AnalysisDisplay::spectrumBounds()const noexcept{auto area=getLocalBounds().toFloat();area=area.removeFromTop(area.getHeight()*.68f);area.removeFromLeft(48.f);area.removeFromBottom(22.f);return area.reduced(8.f,4.f);}
void AnalysisDisplay::paintCursor(juce::Graphics&g){if(!cursorVisible)return;auto spectrum=spectrumBounds();if(!spectrum.contains(cursorPosition))return;auto proportion=juce::jlimit(0.f,1.f,(cursorPosition.x-spectrum.getX())/spectrum.getWidth());auto frequency=FrequencyPlot::frequencyAt(proportion);auto db=juce::jmap(cursorPosition.y,spectrum.getBottom(),spectrum.getY(),-90.f,0.f);g.setColour(juce::Colours::white.withAlpha(.55f));g.drawVerticalLine((int)cursorPosition.x,spectrum.getY(),spectrum.getBottom());g.drawHorizontalLine((int)cursorPosition.y,spectrum.getX(),spectrum.getRight());g.setColour(juce::Colours::white);g.fillEllipse(cursorPosition.x-3.f,cursorPosition.y-3.f,6.f,6.f);auto frequencyText=frequency>=1000.f?juce::String(frequency/1000.f,2)+" kHz":juce::String(juce::roundToInt(frequency))+" Hz";auto text=frequencyText+"   "+juce::String(db,1)+" dB";juce::Rectangle<int>label(0,0,132,22);label.setPosition((int)juce::jlimit(spectrum.getX(),spectrum.getRight()-label.getWidth(),cursorPosition.x+8.f),(int)juce::jlimit(spectrum.getY(),spectrum.getBottom()-label.getHeight(),cursorPosition.y-27.f));g.setColour(juce::Colour(0xee10151d));g.fillRoundedRectangle(label.toFloat(),4.f);g.setColour(juce::Colours::white);g.setFont(11.f);g.drawText(text,label.reduced(6,0),juce::Justification::centredLeft);}
void AnalysisDisplay::drawSourceSpectra(juce::Graphics&g,juce::Rectangle<float>area,float alpha)const
{
    const auto mixColour=UiTheme::mix(state.state),referenceColour=UiTheme::reference(state.state);
    const auto slope=state.getRawParameterValue("analysisSlope")->load();
    const auto listenMode=(int)state.getRawParameterValue("mode")->load();
    const bool showMid=listenMode!=3,showSide=listenMode==3;
    auto draw=[&](const SpectrumAnalyzer&analyzer,juce::Colour colour,bool selected,float fillAlpha,float lineAlpha)
    {
        auto path=FrequencyPlot::spectrumPath(analyzer.getSpectrum(),analyzer.getFftSize(),sampleRate,area,-90.f,0.f,slope);
        if(selected){auto fill=path;fill.lineTo(area.getRight(),area.getBottom());fill.lineTo(area.getX(),area.getBottom());fill.closeSubPath();g.setColour(colour.withAlpha(alpha*fillAlpha));g.fillPath(fill);}
        g.setColour(colour.withAlpha(alpha*lineAlpha));g.strokePath(path,juce::PathStrokeType(selected?1.8f:1.35f));
    };
    if(showMid){draw(mixAnalyzer,mixColour,!referenceSelected,.14f,referenceSelected ? .72f : 1.f);draw(referenceAnalyzer,referenceColour,referenceSelected,.14f,referenceSelected ? 1.f : .72f);}
    if(showSide){draw(mixSideAnalyzer,mixColour,!referenceSelected,.24f,referenceSelected ? .58f : .86f);draw(referenceSideAnalyzer,referenceColour,referenceSelected,.24f,referenceSelected ? .86f : .58f);}
}
void AnalysisDisplay::paint(juce::Graphics&g){
    g.fillAll(juce::Colour(0xff151d27));auto spectrum=spectrumBounds();g.setFont(10);
    for(auto db:{0,-18,-36,-54,-72,-90}){auto y=juce::jmap((float)db,-90.f,0.f,spectrum.getBottom(),spectrum.getY());g.setColour(juce::Colours::white.withAlpha(.08f));g.drawHorizontalLine((int)y,spectrum.getX(),spectrum.getRight());g.setColour(juce::Colours::white.withAlpha(.45f));g.drawText(juce::String(db)+" dB",2,(int)y-6,43,12,juce::Justification::right);}
    for(auto f:{20,50,100,200,500,1000,2000,5000,10000,20000}){auto x=FrequencyPlot::xForFrequency((float)f,spectrum);g.setColour(juce::Colours::white.withAlpha(.07f));g.drawVerticalLine((int)x,spectrum.getY(),spectrum.getBottom());g.setColour(juce::Colours::white.withAlpha(.45f));g.drawText(f>=1000?juce::String(f/1000)+"k":juce::String(f),(int)x-14,(int)spectrum.getBottom()+4,28,12,juce::Justification::centred);}
    auto mixColour=UiTheme::mix(state.state),referenceColour=UiTheme::reference(state.state),eqColour=UiTheme::equalizer();
    const auto listenMode=(int)state.getRawParameterValue("mode")->load();drawSourceSpectra(g,spectrum);
    const bool eqBypassed=state.getRawParameterValue("eqBypass")->load()>.5f;if(!eqBypassed){g.setColour(eqColour);g.strokePath(FrequencyPlot::equalizerPath(state,sampleRate,spectrum),juce::PathStrokeType(1.8f));}
    auto legend=spectrum.toNearestInt().removeFromTop(20);g.setColour(mixColour);g.drawText(referenceSelected?"MIX outline":"MIX selected",legend.removeFromLeft(82),juce::Justification::left);g.setColour(referenceColour);g.drawText(referenceSelected?"REF selected":"REF outline",legend.removeFromLeft(82),juce::Justification::left);g.setColour(juce::Colours::white.withAlpha(.7f));const juce::String modes[]{"STEREO","MONO","MID","SIDE"};g.drawText(modes[juce::jlimit(0,3,listenMode)],legend.removeFromLeft(70),juce::Justification::left);g.setColour(eqBypassed?juce::Colours::white.withAlpha(.45f):eqColour);g.drawText(eqBypassed?"EQ OFF":"EQ",legend.removeFromLeft(45),juce::Justification::left);
    auto scope=getLocalBounds().toFloat();scope.removeFromTop(getHeight()*.68f);scope=scope.reduced(8);juce::Path wave;for(int x=0;x<(int)scope.getWidth();++x){auto index=juce::jlimit(0,scopeSize-1,(int)(x/juce::jmax(1.f,scope.getWidth())*scopeSize));auto y=scope.getCentreY()-outputTime[(size_t)index]*scope.getHeight()*.45f;if(x==0)wave.startNewSubPath(scope.getX(),y);else wave.lineTo(scope.getX()+x,y);}g.setColour(juce::Colours::white.withAlpha(.92f));g.strokePath(wave,juce::PathStrokeType(1));g.setColour(juce::Colours::white.withAlpha(.78f));g.drawText("OUTPUT SCOPE / LISTEN MODE RESULT",scope.toNearestInt().removeFromTop(20),juce::Justification::left);paintCursor(g);
}
}
