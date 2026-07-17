#pragma once
#include <juce_core/juce_core.h>
#include <cstdint>

namespace referencelab {

struct LoopSettings {
    bool enabled = false;
    double startSeconds = 0.0;
    double endSeconds = 0.0;
    double crossfadeMs = 10.0;
    bool isValid() const noexcept { return startSeconds >= 0.0 && endSeconds > startSeconds; }
};

struct ReferenceMetadata {
    juce::String uuid { juce::Uuid().toString() };
    juce::String provider { "local" };
    juce::String title, artist, album, genre, musicalKey, notes, source;
    std::optional<int> year, rating;
    std::optional<double> bpm;
    bool favourite = false;
    juce::StringArray tags;
    double durationSeconds = 0.0, sampleRate = 0.0, startOffsetSeconds = 0.0;
    int channels = 0;
    juce::Time dateAdded { juce::Time::getCurrentTime() }, lastUsed;
    std::int64_t sizeBytes = 0;
    juce::Time lastModified;
    LoopSettings loop;

    juce::var toVar() const;
    static std::optional<ReferenceMetadata> fromVar(const juce::var&, juce::String& error);
    juce::File resolveAgainst(const juce::File& root) const;
};

struct ReferenceLibrary {
    int schemaVersion = 2;
    juce::String id { "local-main" }, name { "Main Reference Library" };
    juce::File root;
    std::vector<ReferenceMetadata> references;
};

struct ReferencePlaylist {
    juce::String id { juce::Uuid().toString() }, name { "Nuova playlist" };
    juce::StringArray referenceIds;
    int currentIndex = -1;
    bool add(const juce::String&); bool remove(const juce::String&);
    juce::String current() const; juce::String selectRelative(int);
    juce::var toVar() const;
    static std::optional<ReferencePlaylist> fromVar(const juce::var&, juce::String& error);
};

} // namespace referencelab
