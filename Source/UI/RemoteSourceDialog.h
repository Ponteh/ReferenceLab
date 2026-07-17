#pragma once

#include <juce_gui_basics/juce_gui_basics.h>
#include <functional>

namespace referencelab {
enum class RemoteSourceType { directAudio,jsonCatalog };

class RemoteSourceDialog {
public:
    using Callback=std::function<void(RemoteSourceType,const juce::String&)>;
    static void show(Callback);
};
}
