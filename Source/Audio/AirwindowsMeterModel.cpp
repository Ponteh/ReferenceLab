#include "AirwindowsMeterModel.h"
#include <algorithm>

namespace referencelab {
void AirwindowsMeterModel::Accumulator::reset()noexcept
{
    rmsL=rmsR=0;peakL=peakR=slewL=slewR=previousL=previousR=0;
    zeroL=zeroR=longestZeroL=longestZeroR=0;wasPositiveL=wasPositiveR=false;count=0;
}

void AirwindowsMeterModel::Accumulator::clearWindow()noexcept
{
    rmsL=rmsR=0;peakL=peakR=slewL=slewR=0;
    zeroL=zeroR=longestZeroL=longestZeroR=0;count=0;
}

void AirwindowsMeterModel::prepare(double value)noexcept
{
    sampleRate=value>0?value:44100.0;reset();
}

void AirwindowsMeterModel::reset()noexcept
{
    displayAccumulator.reset();
    for(auto&accumulator:scoreAccumulators)accumulator.reset();
    displayFifo.reset();scoreFifo.reset();
    boundaryPending.store(true,std::memory_order_release);
    mixScoreResetPending.store(true,std::memory_order_release);
    referenceScoreResetPending.store(true,std::memory_order_release);
    scoreResetMarkers={{true,true}};referenceScoreWasActive=false;
}

void AirwindowsMeterModel::setSource(bool isReference)noexcept
{
    if(sourceReference.exchange(isReference,std::memory_order_acq_rel)!=isReference)markBoundary();
}

bool AirwindowsMeterModel::pushDisplay(const AirwindowsMeterFrame&frame)noexcept
{
    int s1=0,n1=0,s2=0,n2=0;displayFifo.prepareToWrite(1,s1,n1,s2,n2);
    if(n1>0)displayFrames[(size_t)s1]=frame;else if(n2>0)displayFrames[(size_t)s2]=frame;
    displayFifo.finishedWrite(n1+n2);return n1+n2>0;
}

bool AirwindowsMeterModel::pushScore(const AirwindowsMeterFrame&frame)noexcept
{
    int s1=0,n1=0,s2=0,n2=0;scoreFifo.prepareToWrite(1,s1,n1,s2,n2);
    if(n1>0)scoreFrames[(size_t)s1]=frame;else if(n2>0)scoreFrames[(size_t)s2]=frame;
    scoreFifo.finishedWrite(n1+n2);return n1+n2>0;
}

void AirwindowsMeterModel::processBuffer(const juce::AudioBuffer<float>&buffer,Accumulator&state,bool score,bool referenceSource)noexcept
{
    if(buffer.getNumChannels()==0)return;
    const auto rmsSize=1881.0*sampleRate/44100.0;
    const auto zeroScale=(float)(44100.0/sampleRate);
    for(int i=0;i<buffer.getNumSamples();++i)
    {
        const auto l=buffer.getSample(0,i),r=buffer.getNumChannels()>1?buffer.getSample(1,i):l;
        const auto al=std::abs(l),ar=std::abs(r);
        state.rmsL+=al*al;state.rmsR+=ar*ar;
        state.peakL=juce::jmax(state.peakL,al);state.peakR=juce::jmax(state.peakR,ar);
        state.slewL=juce::jmax(state.slewL,std::abs(l-state.previousL)*(float)sampleRate/28000.f);
        state.slewR=juce::jmax(state.slewR,std::abs(r-state.previousR)*(float)sampleRate/28000.f);
        state.previousL=l;state.previousR=r;
        state.zeroL+=zeroScale;state.zeroR+=zeroScale;
        state.longestZeroL=juce::jmax(state.longestZeroL,state.zeroL);state.longestZeroR=juce::jmax(state.longestZeroR,state.zeroR);
        if((state.wasPositiveL&&l<0.f)||(!state.wasPositiveL&&l>0.f)){state.wasPositiveL=!state.wasPositiveL;state.zeroL=0.f;}
        if((state.wasPositiveR&&r<0.f)||(!state.wasPositiveR&&r>0.f)){state.wasPositiveR=!state.wasPositiveR;state.zeroR=0.f;}
        if(++state.count>rmsSize)
        {
            AirwindowsMeterFrame frame;
            frame.rmsL=std::sqrt(std::sqrt((float)(state.rmsL/state.count)));frame.rmsR=std::sqrt(std::sqrt((float)(state.rmsR/state.count)));
            frame.peakL=std::sqrt(state.peakL);frame.peakR=std::sqrt(state.peakR);frame.slewL=state.slewL;frame.slewR=state.slewR;
            frame.zeroL=state.longestZeroL;frame.zeroR=state.longestZeroR;
            if(score)
            {
                const auto index=referenceSource?1u:0u;frame.reference=referenceSource;frame.boundary=scoreResetMarkers[index];
                if(pushScore(frame)&&frame.boundary)scoreResetMarkers[index]=false;
            }
            else
            {
                frame.reference=sourceReference.load(std::memory_order_acquire);
                frame.boundary=boundaryPending.exchange(false,std::memory_order_acq_rel);
                if(!pushDisplay(frame)&&frame.boundary)boundaryPending.store(true,std::memory_order_release);
            }
            state.clearWindow();
        }
    }
}

void AirwindowsMeterModel::process(const juce::AudioBuffer<float>&buffer)noexcept
{
    processBuffer(buffer,displayAccumulator,false,false);
}

void AirwindowsMeterModel::processScores(const juce::AudioBuffer<float>&mix,const juce::AudioBuffer<float>&reference,bool referenceActive)noexcept
{
    if(mixScoreResetPending.exchange(false,std::memory_order_acq_rel)){scoreAccumulators[0].reset();scoreResetMarkers[0]=true;}
    processBuffer(mix,scoreAccumulators[0],true,false);
    if(referenceScoreResetPending.exchange(false,std::memory_order_acq_rel)){scoreAccumulators[1].reset();scoreResetMarkers[1]=true;}
    if(referenceActive)processBuffer(reference,scoreAccumulators[1],true,true);
    else if(referenceScoreWasActive)scoreAccumulators[1].reset();
    referenceScoreWasActive=referenceActive;
}

int AirwindowsMeterModel::pull(AirwindowsMeterFrame*destination,int maximum)noexcept
{
    int s1=0,n1=0,s2=0,n2=0;displayFifo.prepareToRead(juce::jmin(maximum,displayFifo.getNumReady()),s1,n1,s2,n2);
    std::copy_n(displayFrames.data()+s1,n1,destination);std::copy_n(displayFrames.data()+s2,n2,destination+n1);
    displayFifo.finishedRead(n1+n2);return n1+n2;
}

int AirwindowsMeterModel::pullScores(AirwindowsMeterFrame*destination,int maximum)noexcept
{
    int s1=0,n1=0,s2=0,n2=0;scoreFifo.prepareToRead(juce::jmin(maximum,scoreFifo.getNumReady()),s1,n1,s2,n2);
    std::copy_n(scoreFrames.data()+s1,n1,destination);std::copy_n(scoreFrames.data()+s2,n2,destination+n1);
    scoreFifo.finishedRead(n1+n2);return n1+n2;
}
}
