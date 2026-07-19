#include "ComparisonState.h"

namespace referencelab {
ComparisonSettings comparisonSettingsFromState(const juce::AudioProcessorValueTreeState& state) noexcept {
    ComparisonSettings settings;
    settings.bypass = state.getRawParameterValue("eqBypass")->load() > 0.5f;
    settings.highPassEnabled = state.getRawParameterValue("hpfEnabled")->load() > 0.5f;
    settings.bandPassEnabled = state.getRawParameterValue("bellEnabled")->load() > 0.5f;
    settings.lowPassEnabled = state.getRawParameterValue("lpfEnabled")->load() > 0.5f;
    settings.highPassSlope = 12 + 12 * (int)state.getRawParameterValue("hpfSlope")->load();
    settings.lowPassSlope = 12 + 12 * (int)state.getRawParameterValue("lpfSlope")->load();
    settings.highPassHz = state.getRawParameterValue("hpf")->load();
    settings.bandPassHz = state.getRawParameterValue("bellFreq")->load();
    settings.bandPassGainDb = state.getRawParameterValue("bellGain")->load();
    settings.bandPassQ = state.getRawParameterValue("bellQ")->load();
    settings.lowPassHz = state.getRawParameterValue("lpf")->load();
    settings.mode = static_cast<ListeningMode>((int)state.getRawParameterValue("mode")->load());
    settings.swapLeftRight = state.getRawParameterValue("swapLR")->load() > 0.5f;
    return settings;
}
}
