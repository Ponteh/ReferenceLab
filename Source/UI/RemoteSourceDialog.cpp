#include "RemoteSourceDialog.h"

namespace referencelab {
void RemoteSourceDialog::show(Callback callback){
    auto*window=new juce::AlertWindow("Aggiungi sorgente remota","Scegli il tipo di sorgente e inserisci il relativo URL HTTP/HTTPS",juce::MessageBoxIconType::QuestionIcon);
    window->addComboBox("sourceType",{"Audio diretto","Catalogo JSON"},"Tipo");
    if(auto*type=window->getComboBoxComponent("sourceType"))type->setSelectedId(1,juce::dontSendNotification);
    window->addTextEditor("url","","URL");
    window->addTextBlock("Audio: WAV, AIFF, FLAC o MP3. Catalogo: documento JSON compatibile con ReferenceLab.");
    window->addButton("Aggiungi",1,juce::KeyPress(juce::KeyPress::returnKey));
    window->addButton("Annulla",0,juce::KeyPress(juce::KeyPress::escapeKey));
    window->enterModalState(true,juce::ModalCallbackFunction::create([window,callback=std::move(callback)](int result)mutable{
        std::unique_ptr<juce::AlertWindow>owner(window);if(result!=1||!callback)return;
        auto*type=window->getComboBoxComponent("sourceType");auto selected=type!=nullptr&&type->getSelectedId()==2?RemoteSourceType::jsonCatalog:RemoteSourceType::directAudio;
        callback(selected,window->getTextEditorContents("url").trim());
    }),false);
}
}
