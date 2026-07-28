#pragma once
#include <juce_core/juce_core.h>
#include <vector>

namespace referencelab {
enum class HeadphoneFilterType { peaking, lowShelf, highShelf };

struct HeadphoneFilter {
    HeadphoneFilterType type=HeadphoneFilterType::peaking;
    float frequency=1000.f,gainDb=0.f,q=.707f;
    juce::var toVar()const;
    static std::optional<HeadphoneFilter>fromVar(const juce::var&,juce::String&);
};

struct HeadphoneEntry {
    juce::String label,source,form,rig;
    juce::String displayName()const{return label+" — "+source+(rig.isNotEmpty()?" / "+rig:juce::String{});}
};

struct HeadphoneProfile {
    juce::String id,name,headphone,source,form,rig,target;
    float preampDb=0.f;
    std::vector<HeadphoneFilter>filters;
    juce::var toVar()const;
    static std::optional<HeadphoneProfile>fromVar(const juce::var&,juce::String&);
};
}
