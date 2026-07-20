#pragma once
#include <juce_data_structures/juce_data_structures.h>
#include <juce_graphics/juce_graphics.h>

namespace referencelab {
juce::ValueTree createGuiMagicTheme(double libraryWidthRatio);
void setGuiMagicLibraryRatio(juce::ValueTree&tree,double libraryWidthRatio);
void setGuiMagicDrawers(juce::ValueTree&tree,bool libraryOpen,bool settingsOpen,double libraryWidthRatio);
void setGuiMagicSourceColours(juce::ValueTree&tree,juce::Colour mix,juce::Colour reference);
}
