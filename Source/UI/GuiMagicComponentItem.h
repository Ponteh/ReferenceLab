#pragma once

#include <foleys_gui_magic/foleys_gui_magic.h>
#include <functional>

namespace referencelab {
using MagicComponentLookup=std::function<juce::Component*(const juce::String&)>;

void createMagicGuiWithComponents(foleys::MagicGUIBuilder&builder,
                                  juce::Component&parent,
                                  const MagicComponentLookup&lookup);
}
