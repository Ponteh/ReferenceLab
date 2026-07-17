#pragma once
#include "../Audio/AnalysisEngine.h"
#include <juce_gui_basics/juce_gui_basics.h>

namespace referencelab {
class AnalysisMeters final : public juce::Component {
public:
    void setSnapshots(MeterSnapshot mix, MeterSnapshot reference, MeterSnapshot output);
    void setSourceColours(juce::Colour mix, juce::Colour reference) noexcept;
    void paint(juce::Graphics&) override;
private:
    MeterSnapshot mixSnapshot, referenceSnapshot, outputSnapshot;
    juce::Colour mixColour{0xff54c6eb}, referenceColour{0xffff9f43};
    static void drawLevelBar(juce::Graphics&,juce::Rectangle<float>,float,juce::Colour,const juce::String&);
    static void drawSource(juce::Graphics&,juce::Rectangle<float>,const juce::String&,const MeterSnapshot&,juce::Colour);
    static void drawOutput(juce::Graphics&,juce::Rectangle<float>,const MeterSnapshot&);
};
}
