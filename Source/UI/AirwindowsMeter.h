#pragma once
#include "../Audio/AirwindowsMeterModel.h"
#include <juce_events/juce_events.h>
#include <juce_gui_basics/juce_gui_basics.h>
#include <array>

namespace referencelab {
class AirwindowsMeter final : public juce::Component,
                              public juce::SettableTooltipClient,
                              private juce::Timer {
public:
    explicit AirwindowsMeter(AirwindowsMeterModel&);
    void paint(juce::Graphics&) override;
    void mouseDoubleClick(const juce::MouseEvent&) override;
    void setSourceColours(juce::Colour mix,juce::Colour reference);

private:
    static constexpr int historyCapacity=4096;
    static constexpr int scoreBins=16;
    struct ScoreState {
        std::array<std::array<float,scoreBins>,3>bins{};
        std::array<float,3>grades{};
        double cumulative=0.0000001,duration=0.00001;
        juce::Colour barColour{0xff5c6975};
        bool valid=false;
    };
    AirwindowsMeterModel&model;
    std::array<AirwindowsMeterFrame,historyCapacity>history{};
    std::array<ScoreState,2>scores{};
    int writePosition=0,historyCount=0;
    juce::Colour mixColour{0xff39a9d6},referenceColour{0xffff982f};
    void timerCallback()override;
    void append(const AirwindowsMeterFrame&)noexcept;
    void updateScore(const AirwindowsMeterFrame&)noexcept;
};
}
