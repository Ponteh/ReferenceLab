#pragma once
#include <juce_gui_basics/juce_gui_basics.h>

namespace referencelab {
class PanelResizer final : public juce::Component, public juce::SettableTooltipClient {
public:
    PanelResizer() { setMouseCursor(juce::MouseCursor::LeftRightResizeCursor); }
    void paint(juce::Graphics& g) override { g.setColour(juce::Colours::white.withAlpha(isMouseOver() ? .3f : .13f));g.fillRoundedRectangle(getLocalBounds().toFloat().reduced(3.f,24.f),2.f); }
    void mouseEnter(const juce::MouseEvent&) override { repaint(); }
    void mouseExit(const juce::MouseEvent&) override { repaint(); }
    void mouseDown(const juce::MouseEvent& e) override { dragStartX=e.getScreenX();if(onDragStart)onDragStart(); }
    void mouseDrag(const juce::MouseEvent& e) override { if(onDrag)onDrag(e.getScreenX()-dragStartX); }
    void mouseDoubleClick(const juce::MouseEvent&) override { if(onReset)onReset(); }
    std::function<void()>onDragStart,onReset;std::function<void(int)>onDrag;
private:int dragStartX=0;
};
}
