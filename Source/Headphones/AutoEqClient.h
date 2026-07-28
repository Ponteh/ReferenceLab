#pragma once
#include "HeadphoneProfile.h"
#include "../Network/HttpDownloader.h"

namespace referencelab {
class AutoEqClient {
public:
    static std::vector<HeadphoneEntry>search(const juce::String&,juce::String&);
    static std::optional<HeadphoneProfile>generate(const HeadphoneEntry&,const juce::String&profileName,double sampleRate,juce::String&);
    static constexpr const char*serviceUrl="https://autoeq.app";
};
}
