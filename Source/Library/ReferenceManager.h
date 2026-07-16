#pragma once
#include "LocalReferenceProvider.h"
#include "MetadataRepository.h"
#include <mutex>

namespace referencelab {
enum class ReferenceSort { title, artist, genre, bpm, rating, dateAdded, lastUsed };
struct ReferenceFilter {
    juce::String query;
    bool favouritesOnly = false;
    int minimumRating = 0;
    ReferenceSort sort = ReferenceSort::title;
    bool ascending = true;
};
class ReferenceManager {
public:
    explicit ReferenceManager(juce::File database);
    void load(); bool save(juce::String& error);
    bool addFile(const juce::File&,juce::String& error);
    int scanFolder(const juce::File&,bool recursive,juce::String& warning);
    bool remove(const juce::String& uuid,juce::String& error);
    bool updateMetadata(const ReferenceMetadata&,juce::String& error);
    std::vector<ReferenceMetadata> search(const juce::String& query) const;
    std::vector<ReferenceMetadata> filter(const ReferenceFilter&) const;
    ReferenceLibrary snapshot() const;
    juce::String getLastWarning() const;
private:
    mutable std::mutex mutex; MetadataRepository repository; LocalReferenceProvider local; ReferenceLibrary library; juce::String lastWarning;
    static bool matches(const ReferenceMetadata&,const juce::String&);
};
}
