#pragma once

#include "SampleFifo.h"
#include <juce_dsp/juce_dsp.h>
#include <memory>
#include <vector>

namespace referencelab {
class SpectrumAnalyzer {
public:
    explicit SpectrumAnalyzer(int fftOrder = 11);
    void setFftOrder(int);
    void setSmoothing(float value) noexcept { smoothing = juce::jlimit(0.01f, 1.0f, value); }
    bool update(SampleFifo&) noexcept;
    int getFftSize() const noexcept { return fftSize; }
    const std::vector<float>& getSpectrum() const noexcept { return spectrum; }

private:
    std::unique_ptr<juce::dsp::FFT> fft;
    std::unique_ptr<juce::dsp::WindowingFunction<float>> window;
    std::vector<float> timeData, scratch, fftData, spectrum;
    int fftSize = 0;
    float smoothing = 0.22f;
};
}
