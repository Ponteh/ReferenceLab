#pragma once
#include "HeadphoneProfile.h"
#include <mutex>

namespace referencelab {
class HeadphoneProfileRepository {
public:
    explicit HeadphoneProfileRepository(juce::File);
    bool load(juce::String&);bool save(juce::String&)const;
    std::vector<HeadphoneProfile>snapshot()const;
    bool upsert(HeadphoneProfile,juce::String&);bool remove(const juce::String&,juce::String&);
    std::optional<HeadphoneProfile>find(const juce::String&)const;
private:
    juce::File file;mutable std::mutex mutex;std::vector<HeadphoneProfile>profiles;
};
}
