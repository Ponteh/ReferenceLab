#include "GuiMagicComponentItem.h"

namespace referencelab {
namespace {
thread_local const MagicComponentLookup*activeLookup=nullptr;

class ExistingComponentItem final:public foleys::GuiItem {
public:
    ExistingComponentItem(foleys::MagicGUIBuilder&builder,const juce::ValueTree&node)
        :GuiItem(builder,node)
    {
        if(activeLookup!=nullptr)
            component=(*activeLookup)(node.getProperty("id").toString());
        jassert(component!=nullptr);
        addAndMakeVisible(component!=nullptr?*component:fallback);
    }

    void update()override{}
    juce::Component*getWrappedComponent()override{return component!=nullptr?component:&fallback;}
    FOLEYS_DECLARE_GUI_FACTORY(ExistingComponentItem)

private:
    juce::Component*component=nullptr;
    juce::Component fallback;
};
}

void createMagicGuiWithComponents(foleys::MagicGUIBuilder&builder,
                                  juce::Component&parent,
                                  const MagicComponentLookup&lookup)
{
    builder.registerFactory("ReferenceLabComponent",&ExistingComponentItem::factory);
    const auto*previous=activeLookup;
    activeLookup=&lookup;
    builder.createGUI(parent);
    activeLookup=previous;
}
}
