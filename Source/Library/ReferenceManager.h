#pragma once
#include "LocalReferenceProvider.h"
#include "MetadataRepository.h"
#include <mutex>

namespace referencelab {
class ReferenceManager {
public:
    explicit ReferenceManager(juce::File database);
    void load(); bool save(juce::String& error);
    bool addFile(const juce::File&,juce::String& error);
    int scanFolder(const juce::File&,bool recursive,juce::String& warning);
    bool remove(const juce::String& uuid,juce::String& error);
    std::vector<ReferenceMetadata> search(const juce::String& query) const;
    ReferenceLibrary snapshot() const;
    juce::String getLastWarning() const;
private:
    mutable std::mutex mutex; MetadataRepository repository; LocalReferenceProvider local; ReferenceLibrary library; juce::String lastWarning;
    static bool matches(const ReferenceMetadata&,const juce::String&);
};
}
