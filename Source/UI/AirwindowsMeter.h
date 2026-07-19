#pragma once
#include "../Audio/AirwindowsMeterModel.h"
#include <juce_gui_basics/juce_gui_basics.h>
#include <juce_events/juce_events.h>
#include <array>

namespace referencelab {
class AirwindowsMeter final:public juce::Component,public juce::SettableTooltipClient,private juce::Timer{
public:explicit AirwindowsMeter(AirwindowsMeterModel&);void paint(juce::Graphics&)override;void mouseDoubleClick(const juce::MouseEvent&)override;
private:static constexpr int historySize=512;AirwindowsMeterModel&model;std::array<AirwindowsMeterFrame,historySize>history{};int writePosition=0,valid=0;void timerCallback()override;void reset();void drawLane(juce::Graphics&,juce::Rectangle<float>,const juce::String&,int)const;
};
}
