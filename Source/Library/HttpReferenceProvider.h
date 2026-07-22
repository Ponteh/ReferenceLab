#pragma once
#include "IReferenceProvider.h"
namespace referencelab { class HttpReferenceProvider final:public IReferenceProvider { public:juce::String id()const override{return "http";}bool supports(const juce::File&)const override{return false;}std::vector<ReferenceMetadata>scan(const juce::File&,bool,juce::String&w)override{w="Use an HTTP/HTTPS URL";return{};}bool supportsUrl(const juce::URL&)const override;std::optional<ReferenceMetadata>inspectUrl(const juce::URL&,juce::String&)override;static bool isValidAudioUrl(const juce::String&);}; }
