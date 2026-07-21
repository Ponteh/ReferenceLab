#pragma once

#include <juce_data_structures/juce_data_structures.h>
#include <juce_graphics/juce_graphics.h>

namespace referencelab {
class UiTheme {
public:
    static juce::Colour mix(const juce::ValueTree&) noexcept;
    static juce::Colour reference(const juce::ValueTree&) noexcept;
    static juce::Colour equalizer() noexcept { return juce::Colours::white; }
    static juce::Colour palette(int id) noexcept;
    static int paletteId(juce::Colour) noexcept;
    static void setMix(juce::ValueTree&,juce::Colour,juce::UndoManager* = nullptr);
    static void setReference(juce::ValueTree&,juce::Colour,juce::UndoManager* = nullptr);
};
}
