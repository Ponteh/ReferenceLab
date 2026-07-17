#include "JsonCatalogProvider.h"
namespace referencelab {
std::vector<ReferenceMetadata>JsonCatalogProvider::parse(const juce::String&text,juce::String&w)const{std::vector<ReferenceMetadata>out;juce::var parsed;auto result=juce::JSON::parse(text,parsed);if(result.failed()){w="JSON non valido: "+result.getErrorMessage();return out;}auto*a=parsed.getArray();if(!a&&parsed.getDynamicObject())a=parsed.getProperty("references",{}).getArray();if(!a){w="Il catalogo deve contenere un array 'references'";return out;}for(auto&v:*a){juce::String error;auto item=ReferenceMetadata::fromVar(v,error);if(item){item->provider=id();out.push_back(std::move(*item));}else w+=(w.isEmpty()?"":"\n")+error;}return out;}
std::vector<ReferenceMetadata>JsonCatalogProvider::scan(const juce::File&f,bool,juce::String&w){if(!f.existsAsFile()){w="Catalogo JSON non trovato";return{};}return parse(f.loadFileAsString(),w);}
}
