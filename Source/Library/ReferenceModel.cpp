#include "ReferenceModel.h"

namespace referencelab {
namespace {
void put(juce::DynamicObject& o, const char* key, const juce::var& value) { o.setProperty(key, value); }
juce::var nullable(const std::optional<int>& v) { return v ? juce::var(*v) : juce::var(); }
juce::var nullable(const std::optional<double>& v) { return v ? juce::var(*v) : juce::var(); }
}

juce::var ReferenceMetadata::toVar() const {
    auto* o = new juce::DynamicObject();
    put(*o,"uuid",uuid); put(*o,"provider",provider); put(*o,"title",title);
    put(*o,"artist",artist); put(*o,"album",album); put(*o,"genre",genre);
    put(*o,"year",nullable(year)); put(*o,"bpm",nullable(bpm)); put(*o,"key",musicalKey);
    put(*o,"rating",nullable(rating)); put(*o,"favorite",favourite); put(*o,"notes",notes);
    juce::Array<juce::var> tagVars; for (auto& t : tags) tagVars.add(t); put(*o,"tags",tagVars);
    put(*o,"src",source); put(*o,"pathType",juce::File::isAbsolutePath(source) ? "absolute" : "relative");
    put(*o,"durationSeconds",durationSeconds); put(*o,"sampleRate",sampleRate); put(*o,"channels",channels);
    put(*o,"dateAdded",dateAdded.toISO8601(true)); put(*o,"lastUsed",lastUsed.toISO8601(true));
    put(*o,"startOffsetSeconds",startOffsetSeconds);
    auto* l = new juce::DynamicObject(); put(*l,"enabled",loop.enabled); put(*l,"startSeconds",loop.startSeconds);
    put(*l,"endSeconds",loop.endSeconds); put(*l,"crossfadeMs",loop.crossfadeMs); put(*o,"loop",juce::var(l));
    auto* f = new juce::DynamicObject(); put(*f,"sizeBytes",sizeBytes); put(*f,"lastModified",lastModified.toISO8601(true));
    put(*o,"fileInfo",juce::var(f));
    return juce::var(o);
}

std::optional<ReferenceMetadata> ReferenceMetadata::fromVar(const juce::var& v, juce::String& error) {
    auto* o = v.getDynamicObject();
    if (o == nullptr) { error = "Reference non valida"; return {}; }
    ReferenceMetadata m;
    m.uuid = o->getProperty("uuid").toString(); m.provider = o->getProperty("provider").toString();
    m.title = o->getProperty("title").toString(); m.source = o->getProperty("src").toString();
    if (m.uuid.isEmpty() || m.provider.isEmpty() || m.title.isEmpty() || m.source.isEmpty()) {
        error = "Campi obbligatori mancanti (uuid, provider, title, src)"; return {};
    }
    m.artist=o->getProperty("artist").toString(); m.album=o->getProperty("album").toString();
    m.genre=o->getProperty("genre").toString(); m.musicalKey=o->getProperty("key").toString();
    m.notes=o->getProperty("notes").toString(); m.favourite=(bool)o->getProperty("favorite");
    auto readInt=[&](const char* k)->std::optional<int>{ auto x=o->getProperty(k); return x.isVoid()?std::nullopt:std::optional<int>((int)x); };
    auto readDouble=[&](const char* k)->std::optional<double>{ auto x=o->getProperty(k); return x.isVoid()?std::nullopt:std::optional<double>((double)x); };
    m.year=readInt("year"); m.rating=readInt("rating"); m.bpm=readDouble("bpm");
    if (auto* a=o->getProperty("tags").getArray()) for (auto& t:*a) m.tags.add(t.toString());
    m.durationSeconds=(double)o->getProperty("durationSeconds"); m.sampleRate=(double)o->getProperty("sampleRate");
    m.channels=(int)o->getProperty("channels"); m.startOffsetSeconds=(double)o->getProperty("startOffsetSeconds");
    m.dateAdded=juce::Time::fromISO8601(o->getProperty("dateAdded").toString());
    m.lastUsed=juce::Time::fromISO8601(o->getProperty("lastUsed").toString());
    if(auto* l=o->getProperty("loop").getDynamicObject()) { m.loop.enabled=(bool)l->getProperty("enabled"); m.loop.startSeconds=(double)l->getProperty("startSeconds"); m.loop.endSeconds=(double)l->getProperty("endSeconds"); m.loop.crossfadeMs=(double)l->getProperty("crossfadeMs"); }
    if(auto* f=o->getProperty("fileInfo").getDynamicObject()) { m.sizeBytes=static_cast<std::int64_t>(static_cast<juce::int64>(f->getProperty("sizeBytes"))); m.lastModified=juce::Time::fromISO8601(f->getProperty("lastModified").toString()); }
    return m;
}

juce::File ReferenceMetadata::resolveAgainst(const juce::File& root) const {
    return juce::File::isAbsolutePath(source) ? juce::File(source) : root.getChildFile(source);
}
} // namespace referencelab
