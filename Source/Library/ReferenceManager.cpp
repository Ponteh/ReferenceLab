#include "ReferenceManager.h"
#include <algorithm>
namespace referencelab {
ReferenceManager::ReferenceManager(juce::File f):repository(std::move(f)){library.root=repository.getFile().getParentDirectory();}
void ReferenceManager::load(){juce::String warning;auto next=repository.load(warning);std::scoped_lock lock(mutex);library=std::move(next);lastWarning=warning;}
bool ReferenceManager::save(juce::String& error){return repository.save(snapshot(),error);}
bool ReferenceManager::addFile(const juce::File&f,juce::String&error){ReferenceLibrary copy=snapshot();auto root=copy.root.isDirectory()?copy.root:f.getParentDirectory();auto item=local.inspect(f,root,error);if(!item)return false;{std::scoped_lock lock(mutex);if(library.root==juce::File{})library.root=root;auto duplicate=std::find_if(library.references.begin(),library.references.end(),[&](auto&m){return m.resolveAgainst(library.root)==f;});if(duplicate==library.references.end())library.references.push_back(std::move(*item));}return save(error);}
int ReferenceManager::scanFolder(const juce::File&folder,bool recursive,juce::String&warning){auto found=local.scan(folder,recursive,warning);int added=0;{std::scoped_lock lock(mutex);library.root=folder;for(auto&item:found){auto exists=std::find_if(library.references.begin(),library.references.end(),[&](auto&m){return m.resolveAgainst(library.root)==item.resolveAgainst(folder);});if(exists==library.references.end()){library.references.push_back(std::move(item));++added;}}}juce::String error;if(!save(error))warning+=error;return added;}
bool ReferenceManager::remove(const juce::String&id,juce::String&error){{std::scoped_lock lock(mutex);std::erase_if(library.references,[&](auto&m){return m.uuid==id;});}return save(error);}
bool ReferenceManager::matches(const ReferenceMetadata&m,const juce::String&q){auto needle=q.trim().toLowerCase();if(needle.isEmpty())return true;juce::String hay=m.title+" "+m.artist+" "+m.album+" "+m.genre+" "+m.notes+" "+m.tags.joinIntoString(" ");return hay.toLowerCase().contains(needle);}
std::vector<ReferenceMetadata> ReferenceManager::search(const juce::String&q)const{std::scoped_lock lock(mutex);std::vector<ReferenceMetadata> out;for(auto&m:library.references)if(matches(m,q))out.push_back(m);return out;}
ReferenceLibrary ReferenceManager::snapshot()const{std::scoped_lock lock(mutex);return library;}
juce::String ReferenceManager::getLastWarning()const{std::scoped_lock lock(mutex);return lastWarning;}
}
