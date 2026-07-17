#pragma once
#include <juce_data_structures/juce_data_structures.h>

namespace referencelab {
juce::ValueTree createGuiMagicTheme(double libraryWidthRatio);
void setGuiMagicLibraryRatio(juce::ValueTree&tree,double libraryWidthRatio);
}
