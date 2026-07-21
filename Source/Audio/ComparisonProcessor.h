#pragma once
#include <juce_audio_basics/juce_audio_basics.h>
#include <utility>

namespace referencelab {
enum class ListeningMode { stereo, mono, mid, side };
struct ComparisonSettings {
    bool bypass=true,highPassEnabled=true,bandPassEnabled=false,lowPassEnabled=true,swapLeftRight=false;
    float highPassHz=20.f,bandPassHz=2700.f,bandPassGainDb=0.f,bandPassQ=1.f,lowPassHz=20000.f;
    int highPassSlope=12,lowPassSlope=12;ListeningMode mode=ListeningMode::stereo;
};
class ComparisonProcessor {
public:
    void prepare(double sampleRate,int maximumBlockSize)noexcept;void update(const ComparisonSettings&)noexcept;
    void process(juce::AudioBuffer<float>&mix,juce::AudioBuffer<float>&reference)noexcept;
    void processEqualizer(juce::AudioBuffer<float>&mix,juce::AudioBuffer<float>&reference)noexcept;
    void processListeningMode(juce::AudioBuffer<float>&output)noexcept;
    static void applyListeningMode(juce::AudioBuffer<float>&,ListeningMode)noexcept;
    static float responseDecibelsAt(const ComparisonSettings&,float frequency,double sampleRate)noexcept;
private:
    struct Biquad{double b0=1,b1=0,b2=0,a1=0,a2=0,z1=0,z2=0;float process(float x)noexcept{auto y=b0*x+z1;z1=b1*x-a1*y+z2;z2=b2*x-a2*y;return(float)y;}void set(double nb0,double nb1,double nb2,double na0,double na1,double na2)noexcept{b0=nb0/na0;b1=nb1/na0;b2=nb2/na0;a1=na1/na0;a2=na2/na0;}void reset()noexcept{z1=z2=0;}};
    struct Channel{Biquad hp1,hp2,bandPass,lp1,lp2;};struct Chain{Channel channel[2];}mixChain,referenceChain;
    double rate=44100.0;ComparisonSettings settings;ListeningMode previousMode=ListeningMode::stereo,targetMode=ListeningMode::stereo;bool previousSwap=false,targetSwap=false;float modeBlend=1.f,modeStep=0.f,eqWet=0.f,eqStep=0.f;int modeRemaining=0,eqRemaining=0;
    static std::pair<float,float> transformed(float,float,ListeningMode,bool swapLeftRight)noexcept;void processModes(juce::AudioBuffer<float>&,juce::AudioBuffer<float>&)noexcept;void processEq(juce::AudioBuffer<float>&,Chain&,float wet)noexcept;void configureFilters()noexcept;
};
}
