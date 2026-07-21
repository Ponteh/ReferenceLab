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
    AirwindowsMeterModel&model;
    std::array<AirwindowsMeterFrame,historyCapacity>history{};
    int writePosition=0,historyCount=0;
    juce::Colour mixColour{0xff39a9d6},referenceColour{0xffff982f};
    void timerCallback()override;
    void append(const AirwindowsMeterFrame&)noexcept;
};
}
