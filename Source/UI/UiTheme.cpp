#include "UiTheme.h"

namespace referencelab {
namespace {constexpr juce::uint32 defaultMix=0xff54c6eb,defaultReference=0xffff9f43;juce::Colour read(const juce::ValueTree&state,const char*name,juce::uint32 fallback){return juce::Colour((juce::uint32)(juce::int64)state.getProperty(name,(juce::int64)fallback));}}
juce::Colour UiTheme::mix(const juce::ValueTree&state)noexcept{return read(state,"uiMixColour",defaultMix);}
juce::Colour UiTheme::reference(const juce::ValueTree&state)noexcept{return read(state,"uiReferenceColour",defaultReference);}
juce::Colour UiTheme::palette(int id)noexcept{constexpr juce::uint32 colours[]{defaultMix,defaultReference,0xff9be564,0xffc792ea,0xffffd166,0xffff6b6b};return juce::Colour(colours[(size_t)juce::jlimit(1,6,id)-1]);}
int UiTheme::paletteId(juce::Colour colour)noexcept{for(int id=1;id<=6;++id)if(palette(id)==colour)return id;return 1;}
void UiTheme::setMix(juce::ValueTree&state,juce::Colour colour,juce::UndoManager*undo){state.setProperty("uiMixColour",(juce::int64)colour.getARGB(),undo);}
void UiTheme::setReference(juce::ValueTree&state,juce::Colour colour,juce::UndoManager*undo){state.setProperty("uiReferenceColour",(juce::int64)colour.getARGB(),undo);}
}
