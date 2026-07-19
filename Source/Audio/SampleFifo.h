#pragma once
#include <juce_audio_basics/juce_audio_basics.h>
#include <array>
#include <algorithm>

namespace referencelab {
enum class SampleChannelView { mid, side, left, right };

class SampleFifo {
public:
    static constexpr int capacity=32768;
    void push(const juce::AudioBuffer<float>&buffer,SampleChannelView view=SampleChannelView::mid)noexcept{
        if(buffer.getNumChannels()==0)return;
        int start1=0,size1=0,start2=0,size2=0;fifo.prepareToWrite(juce::jmin(buffer.getNumSamples(),fifo.getFreeSpace()),start1,size1,start2,size2);
        auto copy=[&](int start,int count,int source){for(int i=0;i<count;++i){const float l=buffer.getSample(0,source+i);const float r=buffer.getNumChannels()>1?buffer.getSample(1,source+i):l;float value=.5f*(l+r);switch(view){case SampleChannelView::side:value=.5f*(l-r);break;case SampleChannelView::left:value=l;break;case SampleChannelView::right:value=r;break;case SampleChannelView::mid:break;}samples[(size_t)(start+i)]=value;}};
        copy(start1,size1,0);copy(start2,size2,size1);fifo.finishedWrite(size1+size2);
    }
    int pull(float*destination,int maximum)noexcept{
        int start1=0,size1=0,start2=0,size2=0;fifo.prepareToRead(juce::jmin(maximum,fifo.getNumReady()),start1,size1,start2,size2);
        std::copy_n(samples.data()+start1,size1,destination);std::copy_n(samples.data()+start2,size2,destination+size1);fifo.finishedRead(size1+size2);return size1+size2;
    }
private:juce::AbstractFifo fifo{capacity};std::array<float,capacity>samples{};
};
}
