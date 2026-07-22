#pragma once
#include "ReferenceModel.h"

namespace referencelab {
class IReferenceProvider {
public:
    virtual ~IReferenceProvider() = default;
    virtual juce::String id() const = 0;
    virtual bool supports(const juce::File&) const = 0;
    virtual std::vector<ReferenceMetadata> scan(const juce::File& root, bool recursive, juce::String& warning) = 0;
    virtual bool supportsUrl(const juce::URL&) const { return false; }
    virtual std::optional<ReferenceMetadata> inspectUrl(const juce::URL&, juce::String& error) { error = "This provider does not support URLs"; return {}; }
};
}
