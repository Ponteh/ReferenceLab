#include "AutoEqClient.h"
#include <algorithm>

namespace referencelab {
namespace {
juce::String responseText(const HttpDownloadResult&r){return juce::String::fromUTF8((const char*)r.data.getData(),(int)r.data.getSize());}
}
std::vector<HeadphoneEntry>AutoEqClient::search(const juce::String&query,juce::String&error){
    HttpDownloadOptions options;options.maximumBytes=32ll*1024ll*1024ll;options.resourceName="AutoEq headphone index";
    auto downloaded=HttpDownloader::download(juce::URL(juce::String(serviceUrl)+"/entries"),options);
    if(!downloaded.succeeded()){error=downloaded.message;return{};}
    auto parsed=juce::JSON::parse(responseText(downloaded));auto*root=parsed.getDynamicObject();if(!root){error="AutoEq returned an invalid headphone index";return{};}
    std::vector<HeadphoneEntry>result;auto needle=query.trim().toLowerCase();
    for(auto&property:root->getProperties()){
        auto label=property.name.toString();if(needle.isNotEmpty()&&!label.toLowerCase().contains(needle))continue;
        if(auto*items=property.value.getArray())for(auto&item:*items)if(auto*object=item.getDynamicObject()){HeadphoneEntry entry;entry.label=label;entry.source=object->getProperty("source").toString();entry.form=object->getProperty("form").toString();entry.rig=object->getProperty("rig").toString();result.push_back(std::move(entry));}
    }
    std::sort(result.begin(),result.end(),[](auto&a,auto&b){return a.displayName().compareIgnoreCase(b.displayName())<0;});
    if(result.size()>250)result.resize(250);return result;
}
std::optional<HeadphoneProfile>AutoEqClient::generate(const HeadphoneEntry&entry,const juce::String&profileName,double sampleRate,juce::String&error){
    HttpDownloadOptions targetOptions;targetOptions.maximumBytes=4ll*1024ll*1024ll;targetOptions.resourceName="AutoEq targets";
    auto targetsDownload=HttpDownloader::download(juce::URL(juce::String(serviceUrl)+"/targets"),targetOptions);
    if(!targetsDownload.succeeded()){error=targetsDownload.message;return{};}
    auto targets=juce::JSON::parse(responseText(targetsDownload));juce::String targetLabel;
    if(auto*list=targets.getArray()){
        for(auto&target:*list){
            auto*object=target.getDynamicObject();if(!object)continue;
            auto*recommended=object->getProperty("recommended").getArray();if(!recommended)continue;
            for(auto&candidate:*recommended)if(auto*rule=candidate.getDynamicObject()){
                auto source=rule->getProperty("source").toString(),form=rule->getProperty("form").toString(),rig=rule->getProperty("rig").toString();
                if(source==entry.source&&form==entry.form&&(rig.isEmpty()||rig==entry.rig)){targetLabel=object->getProperty("label").toString();if(rig==entry.rig)break;}
            }
            if(targetLabel.isNotEmpty())break;
        }
    }
    if(targetLabel.isEmpty()){error="AutoEq has no recommended target for this measurement";return{};}
    auto*request=new juce::DynamicObject;request->setProperty("name",entry.label);request->setProperty("source",entry.source);request->setProperty("rig",entry.rig);request->setProperty("target",targetLabel);request->setProperty("fs",juce::jlimit(8000,384000,(int)sampleRate));request->setProperty("parametric_eq",true);request->setProperty("parametric_eq_config","8_PEAKING_WITH_SHELVES");request->setProperty("max_gain",12.0);auto*response=new juce::DynamicObject;juce::Array<juce::var>fields;fields.add("frequency");response->setProperty("fr_fields",fields);response->setProperty("fr_f_step",1.1);request->setProperty("response",response);
    HttpDownloadOptions options;options.maximumBytes=8ll*1024ll*1024ll;options.timeoutMs=30000;options.resourceName="AutoEq profile";options.postData=juce::JSON::toString(juce::var(request));options.contentType="application/json";
    auto downloaded=HttpDownloader::download(juce::URL(juce::String(serviceUrl)+"/equalize"),options);if(!downloaded.succeeded()){error=downloaded.message;return{};}
    auto parsed=juce::JSON::parse(responseText(downloaded));auto*root=parsed.getDynamicObject();auto*peq=root?root->getProperty("parametric_eq").getDynamicObject():nullptr;if(!peq){error="AutoEq returned no parametric EQ";return{};}
    HeadphoneProfile profile;profile.id=juce::Uuid().toString();profile.name=profileName.trim().isNotEmpty()?profileName.trim():entry.label;profile.headphone=entry.label;profile.source=entry.source;profile.form=entry.form;profile.rig=entry.rig;profile.target=targetLabel;profile.preampDb=(float)peq->getProperty("preamp");
    if(auto*filters=peq->getProperty("filters").getArray())for(auto&item:*filters)if(auto*object=item.getDynamicObject()){HeadphoneFilter f;auto type=object->getProperty("type").toString();f.type=type=="LOW_SHELF"?HeadphoneFilterType::lowShelf:type=="HIGH_SHELF"?HeadphoneFilterType::highShelf:HeadphoneFilterType::peaking;f.frequency=(float)object->getProperty("fc");f.gainDb=(float)object->getProperty("gain");f.q=(float)object->getProperty("q");profile.filters.push_back(f);}
    if(profile.filters.empty()){error="AutoEq returned an empty parametric EQ";return{};}return profile;
}
}
