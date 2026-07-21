#include "AirwindowsMeterModel.h"
#include <algorithm>

namespace referencelab {
void AirwindowsMeterModel::reset()noexcept{rmsL=rmsR=0;peakL=peakR=slewL=slewR=previousL=previousR=zeroL=zeroR=longestZeroL=longestZeroR=0;wasPositiveL=wasPositiveR=false;count=0;boundaryPending.store(true,std::memory_order_release);}
void AirwindowsMeterModel::setSource(bool isReference)noexcept{if(sourceReference.exchange(isReference,std::memory_order_acq_rel)!=isReference)markBoundary();}
void AirwindowsMeterModel::push(const AirwindowsMeterFrame&frame)noexcept{int s1=0,n1=0,s2=0,n2=0;fifo.prepareToWrite(1,s1,n1,s2,n2);if(n1>0)frames[(size_t)s1]=frame;else if(n2>0)frames[(size_t)s2]=frame;fifo.finishedWrite(n1+n2);}
void AirwindowsMeterModel::process(const juce::AudioBuffer<float>&buffer)noexcept
{
    if(buffer.getNumChannels()==0)return;
    const auto rmsSize=1881.0*sampleRate/44100.0;
    const auto zeroScale=(float)(44100.0/sampleRate);
    for(int i=0;i<buffer.getNumSamples();++i)
    {
        const auto l=buffer.getSample(0,i),r=buffer.getNumChannels()>1?buffer.getSample(1,i):l,al=std::abs(l),ar=std::abs(r);
        rmsL+=al*al;rmsR+=ar*ar;peakL=juce::jmax(peakL,al);peakR=juce::jmax(peakR,ar);
        slewL=juce::jmax(slewL,std::abs(l-previousL)*(float)sampleRate/28000.f);slewR=juce::jmax(slewR,std::abs(r-previousR)*(float)sampleRate/28000.f);previousL=l;previousR=r;
        zeroL+=zeroScale;zeroR+=zeroScale;longestZeroL=juce::jmax(longestZeroL,zeroL);longestZeroR=juce::jmax(longestZeroR,zeroR);
        if((wasPositiveL&&l<0.f)||(!wasPositiveL&&l>0.f)){wasPositiveL=!wasPositiveL;zeroL=0.f;}
        if((wasPositiveR&&r<0.f)||(!wasPositiveR&&r>0.f)){wasPositiveR=!wasPositiveR;zeroR=0.f;}
        if(++count>rmsSize)
        {
            AirwindowsMeterFrame f;f.rmsL=std::sqrt(std::sqrt((float)(rmsL/count)));f.rmsR=std::sqrt(std::sqrt((float)(rmsR/count)));f.peakL=std::sqrt(peakL);f.peakR=std::sqrt(peakR);f.slewL=slewL;f.slewR=slewR;f.zeroL=longestZeroL;f.zeroR=longestZeroR;f.reference=sourceReference.load(std::memory_order_acquire);f.boundary=boundaryPending.exchange(false,std::memory_order_acq_rel);push(f);
            rmsL=rmsR=0;peakL=peakR=slewL=slewR=zeroL=zeroR=longestZeroL=longestZeroR=0;count=0;
        }
    }
}
int AirwindowsMeterModel::pull(AirwindowsMeterFrame*destination,int maximum)noexcept{int s1=0,n1=0,s2=0,n2=0;fifo.prepareToRead(juce::jmin(maximum,fifo.getNumReady()),s1,n1,s2,n2);std::copy_n(frames.data()+s1,n1,destination);std::copy_n(frames.data()+s2,n2,destination+n1);fifo.finishedRead(n1+n2);return n1+n2;}
}
