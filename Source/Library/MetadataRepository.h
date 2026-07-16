#pragma once
#include "ReferenceModel.h"

namespace referencelab {
class MetadataRepository {
public:
    explicit MetadataRepository(juce::File databaseFile);
    ReferenceLibrary load(juce::String& warning) const;
    bool save(const ReferenceLibrary&, juce::String& error) const;
    const juce::File& getFile() const noexcept { return file; }
private:
    juce::File file;
};
}

