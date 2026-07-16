#pragma once
#include <juce_audio_formats/juce_audio_formats.h>
#include <atomic>
#include <memory>

namespace referencelab {
struct ReferenceAudioData {
    juce::AudioBuffer<float> samples;
    double sampleRate = 44100.0;
    std::size_t bytes() const noexcept { return static_cast<std::size_t>(samples.getNumChannels()) * static_cast<std::size_t>(samples.getNumSamples()) * sizeof(float); }
};
class ReferencePlayer {
public:
    bool load(const juce::File&, juce::AudioFormatManager&, juce::String& error);
    void setAudio(std::shared_ptr<ReferenceAudioData>) noexcept;
    void prepare(double hostSampleRate) noexcept;
    void process(juce::AudioBuffer<float>&) noexcept;
    void play() noexcept { playing.store(true); }
    void pause() noexcept { playing.store(false); }
    void stop() noexcept;
    void seek(double seconds) noexcept;
    void setStartOffset(double seconds) noexcept;
    void setLoop(bool enabled, double startSeconds, double endSeconds) noexcept;
    bool isLoaded() const noexcept { return audio.load(std::memory_order_acquire) != nullptr; }
    bool isPlaying() const noexcept { return playing.load(); }
    double getPositionSeconds() const noexcept;
    double getDurationSeconds() const noexcept;
private:
    std::atomic<std::shared_ptr<ReferenceAudioData>> audio;
    std::atomic<bool> playing { false }, loopEnabled { false };
    std::atomic<double> position { 0.0 }, hostRate { 44100.0 }, startOffset { 0.0 }, loopStart { 0.0 }, loopEnd { 0.0 };
};
}
