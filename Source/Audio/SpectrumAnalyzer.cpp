#include "SpectrumAnalyzer.h"
#include <algorithm>

namespace referencelab {
SpectrumAnalyzer::SpectrumAnalyzer(int order) { setFftOrder(order); }

void SpectrumAnalyzer::setFftOrder(int order) {
    order = juce::jlimit(10, 15, order);
    fftSize = 1 << order;
    fft = std::make_unique<juce::dsp::FFT>(order);
    window = std::make_unique<juce::dsp::WindowingFunction<float>>(
        fftSize, juce::dsp::WindowingFunction<float>::hann);
    timeData.assign((size_t)fftSize, 0.0f);
    scratch.assign((size_t)fftSize, 0.0f);
    fftData.assign((size_t)fftSize * 2, 0.0f);
    spectrum.assign((size_t)fftSize / 2, -100.0f);
}

bool SpectrumAnalyzer::update(SampleFifo& fifo) noexcept {
    const auto count = fifo.pull(scratch.data(), fftSize);
    if (count <= 0) return false;
    if (count >= fftSize)
        std::copy_n(scratch.end() - fftSize, fftSize, timeData.begin());
    else {
        std::move(timeData.begin() + count, timeData.end(), timeData.begin());
        std::copy_n(scratch.begin(), count, timeData.end() - count);
    }

    std::fill(fftData.begin(), fftData.end(), 0.0f);
    std::copy(timeData.begin(), timeData.end(), fftData.begin());
    window->multiplyWithWindowingTable(fftData.data(), fftSize);
    fft->performFrequencyOnlyForwardTransform(fftData.data());
    for (int bin = 0; bin < fftSize / 2; ++bin) {
        const auto db = juce::Decibels::gainToDecibels(
            fftData[(size_t)bin] / (float)fftSize, -100.0f);
        spectrum[(size_t)bin] += smoothing * (db - spectrum[(size_t)bin]);
    }
    return true;
}
}
