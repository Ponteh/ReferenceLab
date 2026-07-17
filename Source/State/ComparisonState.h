#pragma once

#include "../Audio/ComparisonProcessor.h"
#include <juce_audio_processors/juce_audio_processors.h>

namespace referencelab {
ComparisonSettings comparisonSettingsFromState(const juce::AudioProcessorValueTreeState&) noexcept;
}
