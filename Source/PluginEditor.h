#pragma once
#include <juce_gui_extra/juce_gui_extra.h>
#include "PluginProcessor.h"
class ReferenceLabEditor final:public juce::AudioProcessorEditor{
public: explicit ReferenceLabEditor(ReferenceLabAudioProcessor&);void paint(juce::Graphics&)override;void resized()override;
private: ReferenceLabAudioProcessor&p;juce::TextButton add{"Aggiungi reference"},mix{"MIX"},ref{"REFERENCE"};juce::Label title;
};
