#pragma once
#include <juce_dsp/juce_dsp.h>

namespace referencelab {
enum class ListeningMode { stereo, mono, mid, side };
struct ComparisonSettings { bool bypass=true; float highPassHz=20.f, bellHz=1000.f, bellGainDb=0.f, bellQ=1.f, lowPassHz=20000.f; ListeningMode mode=ListeningMode::stereo; };
class ComparisonProcessor {
public:
    void prepare(double sampleRate, int maximumBlockSize);
    void update(const ComparisonSettings&);
    void process(juce::AudioBuffer<float>& mix, juce::AudioBuffer<float>& reference) noexcept;
    static void applyListeningMode(juce::AudioBuffer<float>&, ListeningMode) noexcept;
private:
    using Filter=juce::dsp::IIR::Filter<float>;
    struct Chain { Filter hp[2], bell[2], lp[2]; } mixChain, referenceChain;
    double rate=44100.0; ComparisonSettings settings; bool initialised=false;
    void processChain(juce::AudioBuffer<float>&, Chain&) noexcept;
};
}
