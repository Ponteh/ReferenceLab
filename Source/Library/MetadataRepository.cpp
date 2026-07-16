#include "MetadataRepository.h"

namespace referencelab {
MetadataRepository::MetadataRepository(juce::File f) : file(std::move(f)) {}

ReferenceLibrary MetadataRepository::load(juce::String& warning) const {
    ReferenceLibrary lib; lib.root = file.getParentDirectory();
    if (!file.existsAsFile()) return lib;
    auto parsed = juce::JSON::parse(file);
    auto* root = parsed.getDynamicObject();
    if (root == nullptr) { warning = "reference.json non valido; uso libreria vuota"; return lib; }
    lib.schemaVersion=(int)root->getProperty("schemaVersion");
    if (lib.schemaVersion > 2) { warning = "Schema reference.json più recente di questa versione"; return lib; }
    lib.id=root->getProperty("libraryId").toString(); lib.name=root->getProperty("libraryName").toString();
    auto rootPath=root->getProperty("rootPath").toString(); if(rootPath.isNotEmpty()) lib.root=juce::File(rootPath);
    if(auto* refs=root->getProperty("references").getArray()) for(auto& v:*refs) { juce::String error; if(auto m=ReferenceMetadata::fromVar(v,error)) lib.references.push_back(std::move(*m)); else warning += error + "\n"; }
    lib.schemaVersion=2; return lib;
}

bool MetadataRepository::save(const ReferenceLibrary& lib, juce::String& error) const {
    auto* o=new juce::DynamicObject(); o->setProperty("schemaVersion",2); o->setProperty("libraryId",lib.id);
    o->setProperty("libraryName",lib.name); o->setProperty("rootPath",lib.root.getFullPathName());
    juce::Array<juce::var> refs; for(auto& r:lib.references) refs.add(r.toVar()); o->setProperty("references",refs);
    file.getParentDirectory().createDirectory();
    auto temp=file.getSiblingFile(file.getFileName()+".tmp");
    if(!temp.replaceWithText(juce::JSON::toString(juce::var(o),true))) { error="Impossibile scrivere il file temporaneo"; return false; }
    if(file.existsAsFile()) file.copyFileTo(file.getSiblingFile("reference.json.bak"));
    if(!temp.moveFileTo(file)) { error="Impossibile sostituire reference.json"; return false; }
    return true;
}
} // namespace referencelab
