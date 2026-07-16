#pragma once
#include "IReferenceProvider.h"
#include <juce_audio_formats/juce_audio_formats.h>

namespace referencelab {
class LocalReferenceProvider final : public IReferenceProvider {
public:
    LocalReferenceProvider();
    juce::String id() const override { return "local"; }
    bool supports(const juce::File&) const override;
    std::vector<ReferenceMetadata> scan(const juce::File&, bool, juce::String&) override;
    std::optional<ReferenceMetadata> inspect(const juce::File&, const juce::File& root, juce::String& error);
private:
    juce::AudioFormatManager formats;
};
}
