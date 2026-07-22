#include "LocalReferenceProvider.h"

namespace referencelab {
LocalReferenceProvider::LocalReferenceProvider(){ formats.registerBasicFormats(); }
bool LocalReferenceProvider::supports(const juce::File& f) const { return juce::StringArray{".wav",".aif",".aiff",".flac",".mp3"}.contains(f.getFileExtension().toLowerCase()); }
std::optional<ReferenceMetadata> LocalReferenceProvider::inspect(const juce::File& f,const juce::File& root,juce::String& error){
    if(!f.existsAsFile()){error="Missing file: "+f.getFullPathName();return{};} if(!supports(f)){error="Unsupported format: "+f.getFileName();return{};}
    std::unique_ptr<juce::AudioFormatReader> reader(formats.createReaderFor(f)); if(!reader){error="Decoding failed: "+f.getFileName();return{};}
    ReferenceMetadata m; m.title=f.getFileNameWithoutExtension();m.source=f.isAChildOf(root)?f.getRelativePathFrom(root):f.getFullPathName();m.durationSeconds=reader->lengthInSamples/reader->sampleRate;m.sampleRate=reader->sampleRate;m.channels=(int)reader->numChannels;m.sizeBytes=f.getSize();m.lastModified=f.getLastModificationTime();return m;
}
std::vector<ReferenceMetadata> LocalReferenceProvider::scan(const juce::File& root,bool recursive,juce::String& warning){
    std::vector<ReferenceMetadata> result; if(!root.isDirectory()){warning="Invalid library folder";return result;}
    for(auto entry:juce::RangedDirectoryIterator(root,recursive,"*",juce::File::findFiles)){auto file=entry.getFile();if(!supports(file))continue;juce::String error;if(auto m=inspect(file,root,error))result.push_back(std::move(*m));else warning+=error+"\n";} return result;
}
}
