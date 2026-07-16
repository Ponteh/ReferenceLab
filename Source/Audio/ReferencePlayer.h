#pragma once
#include <juce_audio_formats/juce_audio_formats.h>
#include <atomic>
#include <memory>

namespace referencelab {
class ReferencePlayer {
public:
    bool load(const juce::File&, juce::AudioFormatManager&, juce::String& error);
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
    struct AudioData { juce::AudioBuffer<float> samples; double sampleRate = 44100.0; };
    std::atomic<std::shared_ptr<AudioData>> audio;
    std::atomic<bool> playing { false }, loopEnabled { false };
    std::atomic<double> position { 0.0 }, hostRate { 44100.0 }, startOffset { 0.0 }, loopStart { 0.0 }, loopEnd { 0.0 };
};
}
