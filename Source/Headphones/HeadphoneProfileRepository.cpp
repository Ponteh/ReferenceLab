#include "HeadphoneProfileRepository.h"
#include <algorithm>

namespace referencelab {
HeadphoneProfileRepository::HeadphoneProfileRepository(juce::File value):file(std::move(value)){}
bool HeadphoneProfileRepository::load(juce::String&error){if(!file.existsAsFile())return true;auto parsed=juce::JSON::parse(file.loadFileAsString());auto*root=parsed.getDynamicObject();if(!root){error="Invalid headphone profile database";return false;}std::vector<HeadphoneProfile>loaded;if(auto*a=root->getProperty("profiles").getArray())for(auto&item:*a){auto p=HeadphoneProfile::fromVar(item,error);if(!p)return false;loaded.push_back(std::move(*p));}std::scoped_lock lock(mutex);profiles=std::move(loaded);return true;}
bool HeadphoneProfileRepository::save(juce::String&error)const{juce::Array<juce::var>a;{std::scoped_lock lock(mutex);for(auto&p:profiles)a.add(p.toVar());}auto*root=new juce::DynamicObject;root->setProperty("schemaVersion",1);root->setProperty("profiles",a);file.getParentDirectory().createDirectory();juce::TemporaryFile temporary(file);if(!temporary.getFile().replaceWithText(juce::JSON::toString(juce::var(root),true))||!temporary.overwriteTargetFileWithTemporary()){error="Unable to save headphone profiles";return false;}return true;}
std::vector<HeadphoneProfile>HeadphoneProfileRepository::snapshot()const{std::scoped_lock lock(mutex);return profiles;}
std::optional<HeadphoneProfile>HeadphoneProfileRepository::find(const juce::String&id)const{std::scoped_lock lock(mutex);for(auto&p:profiles)if(p.id==id)return p;return{};}
bool HeadphoneProfileRepository::upsert(HeadphoneProfile p,juce::String&error){{std::scoped_lock lock(mutex);auto found=std::find_if(profiles.begin(),profiles.end(),[&](auto&x){return x.id==p.id;});if(found==profiles.end())profiles.push_back(std::move(p));else*found=std::move(p);}return save(error);}
bool HeadphoneProfileRepository::remove(const juce::String&id,juce::String&error){{std::scoped_lock lock(mutex);profiles.erase(std::remove_if(profiles.begin(),profiles.end(),[&](auto&p){return p.id==id;}),profiles.end());}return save(error);}
}
