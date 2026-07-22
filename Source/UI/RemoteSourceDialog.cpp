#include "RemoteSourceDialog.h"

namespace referencelab {
void RemoteSourceDialog::show(Callback callback){
    auto*window=new juce::AlertWindow("Add Remote Source","Choose the source type and enter its HTTP/HTTPS URL",juce::MessageBoxIconType::QuestionIcon);
    window->addComboBox("sourceType",{"Direct Audio","JSON Catalog"},"Type");
    if(auto*type=window->getComboBoxComponent("sourceType"))type->setSelectedId(1,juce::dontSendNotification);
    window->addTextEditor("url","","URL");
    window->addTextBlock("Audio: WAV, AIFF, FLAC or MP3. Catalog: a ReferenceLab-compatible JSON document.");
    window->addButton("Add",1,juce::KeyPress(juce::KeyPress::returnKey));
    window->addButton("Cancel",0,juce::KeyPress(juce::KeyPress::escapeKey));
    window->enterModalState(true,juce::ModalCallbackFunction::create([window,callback=std::move(callback)](int result)mutable{
        std::unique_ptr<juce::AlertWindow>owner(window);if(result!=1||!callback)return;
        auto*type=window->getComboBoxComponent("sourceType");auto selected=type!=nullptr&&type->getSelectedId()==2?RemoteSourceType::jsonCatalog:RemoteSourceType::directAudio;
        callback(selected,window->getTextEditorContents("url").trim());
    }),false);
}
}
