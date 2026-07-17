#pragma once
#include "IReferenceProvider.h"
namespace referencelab { class JsonCatalogProvider final:public IReferenceProvider { public: juce::String id()const override{return "json-catalog";} bool supports(const juce::File&f)const override{return f.hasFileExtension("json");} std::vector<ReferenceMetadata> scan(const juce::File&,bool,juce::String&)override; }; }
