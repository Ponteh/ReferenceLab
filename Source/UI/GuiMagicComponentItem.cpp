#include "GuiMagicComponentItem.h"
#include <map>

namespace referencelab {
namespace {
juce::CriticalSection lookupLock;
std::map<foleys::MagicGUIBuilder*,MagicComponentLookup> componentLookups;

juce::Component*findRegisteredComponent(foleys::MagicGUIBuilder&builder,const juce::String&id)
{
    MagicComponentLookup lookup;
    {
        const juce::ScopedLock lock(lookupLock);
        const auto found=componentLookups.find(&builder);
        if(found==componentLookups.end())return nullptr;
        lookup=found->second;
    }
    return lookup?lookup(id):nullptr;
}

class ExistingComponentItem final:public foleys::GuiItem {
public:
    ExistingComponentItem(foleys::MagicGUIBuilder&builder,const juce::ValueTree&node)
        :GuiItem(builder,node)
    {
        component=findRegisteredComponent(builder,node.getProperty("id").toString());
        jassert(component!=nullptr);
        if(dynamic_cast<juce::Slider*>(component)!=nullptr)
            setColourTranslation({{"slider-background",juce::Slider::backgroundColourId},{"slider-thumb",juce::Slider::thumbColourId},{"slider-track",juce::Slider::trackColourId},{"rotary-fill",juce::Slider::rotarySliderFillColourId},{"rotary-outline",juce::Slider::rotarySliderOutlineColourId},{"slider-text",juce::Slider::textBoxTextColourId},{"slider-text-background",juce::Slider::textBoxBackgroundColourId},{"slider-text-highlight",juce::Slider::textBoxHighlightColourId},{"slider-text-outline",juce::Slider::textBoxOutlineColourId}});
        else if(dynamic_cast<juce::TextButton*>(component)!=nullptr)
            setColourTranslation({{"button-color",juce::TextButton::buttonColourId},{"button-on-color",juce::TextButton::buttonOnColourId},{"button-off-text",juce::TextButton::textColourOffId},{"button-on-text",juce::TextButton::textColourOnId}});
        else if(dynamic_cast<juce::ToggleButton*>(component)!=nullptr)
            setColourTranslation({{"toggle-text",juce::ToggleButton::textColourId},{"toggle-tick",juce::ToggleButton::tickColourId},{"toggle-tick-disabled",juce::ToggleButton::tickDisabledColourId}});
        else if(dynamic_cast<juce::ComboBox*>(component)!=nullptr)
            setColourTranslation({{"combo-background",juce::ComboBox::backgroundColourId},{"combo-text",juce::ComboBox::textColourId},{"combo-outline",juce::ComboBox::outlineColourId},{"combo-button",juce::ComboBox::buttonColourId},{"combo-arrow",juce::ComboBox::arrowColourId},{"combo-focused-outline",juce::ComboBox::focusedOutlineColourId}});
        else if(dynamic_cast<juce::Label*>(component)!=nullptr)
            setColourTranslation({{"label-background",juce::Label::backgroundColourId},{"label-outline",juce::Label::outlineColourId},{"label-text",juce::Label::textColourId}});
        else if(dynamic_cast<juce::TextEditor*>(component)!=nullptr)
            setColourTranslation({{"editor-background",juce::TextEditor::backgroundColourId},{"editor-text",juce::TextEditor::textColourId},{"editor-outline",juce::TextEditor::outlineColourId},{"editor-focused-outline",juce::TextEditor::focusedOutlineColourId},{"editor-highlight",juce::TextEditor::highlightColourId},{"editor-highlighted-text",juce::TextEditor::highlightedTextColourId}});
        else if(dynamic_cast<juce::ListBox*>(component)!=nullptr)
            setColourTranslation({{"list-background",juce::ListBox::backgroundColourId},{"list-outline",juce::ListBox::outlineColourId},{"list-text",juce::ListBox::textColourId}});
        addAndMakeVisible(component!=nullptr?*component:fallback);
    }

    void update()override
    {
        if(auto*slider=dynamic_cast<juce::Slider*>(component))
        {
            const auto style=getProperty("control-style").toString();
            if(style=="knob")
            {
                slider->setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
                slider->setTextBoxStyle(juce::Slider::TextBoxBelow,false,72,20);
            }
            else
            {
                slider->setSliderStyle(juce::Slider::LinearHorizontal);
                slider->setTextBoxStyle(juce::Slider::TextBoxRight,false,68,22);
            }
            slider->setScrollWheelEnabled(false);
        }
        if(component!=nullptr)
        {
            const auto caption=getProperty("caption").toString();
            if(caption.isNotEmpty())component->setTitle(caption);
        }
    }
    juce::Component*getWrappedComponent()override{return component!=nullptr?component:&fallback;}
    FOLEYS_DECLARE_GUI_FACTORY(ExistingComponentItem)

private:
    juce::Component*component=nullptr;
    juce::Component fallback;
};
}

void createMagicGuiWithComponents(foleys::MagicGUIBuilder&builder,
                                  juce::Component&parent,
                                  MagicComponentLookup lookup)
{
    {
        const juce::ScopedLock lock(lookupLock);
        componentLookups[&builder]=std::move(lookup);
    }
    builder.registerFactory("ReferenceLabComponent",&ExistingComponentItem::factory);
    builder.createGUI(parent);
}

void releaseMagicGuiComponents(foleys::MagicGUIBuilder&builder) noexcept
{
    const juce::ScopedLock lock(lookupLock);
    componentLookups.erase(&builder);
}
}
