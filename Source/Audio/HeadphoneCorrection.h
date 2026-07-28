#pragma once
#include "../Headphones/HeadphoneProfile.h"
#include <juce_dsp/juce_dsp.h>
#include <atomic>

namespace referencelab {
class HeadphoneCorrection {
public:
    void prepare(double,int);
    void setProfile(const std::optional<HeadphoneProfile>&);
    void setEnabled(bool value)noexcept{enabled.store(value);}
    bool isEnabled()const noexcept{return enabled.load();}
    void process(juce::AudioBuffer<float>&)noexcept;
private:
    struct Stage{std::array<juce::dsp::IIR::Filter<float>,2>channels;};
    struct Engine{float preamp=1.f;std::vector<Stage>stages;};
    double sampleRate=44100.0;std::atomic<bool>enabled{false},pendingClear{false};std::atomic<std::shared_ptr<Engine>>pending;std::shared_ptr<Engine>active;
};
}
