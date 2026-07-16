#pragma once
#include "ReferencePlayer.h"
#include <juce_events/juce_events.h>
#include <functional>
#include <list>
#include <mutex>
#include <unordered_map>

namespace referencelab {
enum class CacheState { notLoaded, loading, cached, error };
class CacheManager final : private juce::AsyncUpdater {
public:
    using Callback=std::function<void(std::shared_ptr<ReferenceAudioData>,const juce::String&,bool)>;
    explicit CacheManager(std::size_t maximumBytes=512u*1024u*1024u);
    ~CacheManager() override;
    void loadAsync(const juce::File&,Callback);
    void setMaximumBytes(std::size_t);
    std::size_t getMaximumBytes()const; std::size_t getCurrentBytes()const;
    CacheState getState(const juce::File&)const;
    void clear();
private:
    struct Entry{std::shared_ptr<ReferenceAudioData>data;juce::Time modified;std::size_t bytes=0;std::uint64_t use=0;};
    struct Completion{juce::File file;std::shared_ptr<ReferenceAudioData>data;juce::String error;Callback callback;};
    juce::ThreadPool pool{2}; mutable std::mutex mutex;std::unordered_map<std::string,Entry>entries;std::unordered_map<std::string,CacheState>states;std::list<Completion>completions;std::size_t maximum,current=0;std::uint64_t counter=0;
    static std::string key(const juce::File&);static std::shared_ptr<ReferenceAudioData>decode(const juce::File&,juce::String&);
    void evictLocked();void handleAsyncUpdate()override;
};
}
