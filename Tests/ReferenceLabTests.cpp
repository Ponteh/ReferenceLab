#include <juce_core/juce_core.h>
#include "Library/ReferenceModel.h"
#include "Audio/ComparisonProcessor.h"
#include "Audio/ReferencePlayer.h"

namespace {
class ReferenceModelTests final : public juce::UnitTest {
public:
    ReferenceModelTests() : juce::UnitTest("Reference model", "ReferenceLab") {}

    void runTest() override {
        beginTest("Loop range validation");
        referencelab::LoopSettings loop;
        loop.startSeconds = 10.0;
        loop.endSeconds = 20.0;
        expect(loop.isValid());
        loop.endSeconds = 5.0;
        expect(!loop.isValid());

        beginTest("Metadata JSON round trip");
        referencelab::ReferenceMetadata input;
        input.uuid = "test-uuid";
        input.provider = "local";
        input.title = "Reference";
        input.source = "audio/reference.wav";
        input.artist = juce::String::fromUTF8("Artista è");
        input.bpm = 120.0;
        input.tags.add("mix");

        juce::String error;
        auto output = referencelab::ReferenceMetadata::fromVar(input.toVar(), error);
        expect(output.has_value(), error);
        if (output) {
            expectEquals(output->uuid, input.uuid);
            expectEquals(output->artist, input.artist);
            expectEquals(output->source, input.source);
            expectWithinAbsoluteError(*output->bpm, 120.0, 0.001);
        }

        beginTest("Required fields are rejected");
        auto* invalid = new juce::DynamicObject();
        invalid->setProperty("title", "Incomplete");
        expect(!referencelab::ReferenceMetadata::fromVar(juce::var(invalid), error).has_value());

        beginTest("Mono listening mode");
        juce::AudioBuffer<float> signal(2, 1);
        signal.setSample(0,0,1.0f); signal.setSample(1,0,-0.5f);
        referencelab::ComparisonProcessor::applyListeningMode(signal,referencelab::ListeningMode::mono);
        expectWithinAbsoluteError(signal.getSample(0,0),0.25f,0.0001f);
        expectWithinAbsoluteError(signal.getSample(1,0),0.25f,0.0001f);

        beginTest("Side listening mode");
        signal.setSample(0,0,1.0f); signal.setSample(1,0,-1.0f);
        referencelab::ComparisonProcessor::applyListeningMode(signal,referencelab::ListeningMode::side);
        expectWithinAbsoluteError(signal.getSample(0,0),1.0f,0.0001f);
        expectWithinAbsoluteError(signal.getSample(1,0),-1.0f,0.0001f);

        beginTest("Reference player renders cached audio");
        auto audio=std::make_shared<referencelab::ReferenceAudioData>();
        audio->sampleRate=48000.0;audio->samples.setSize(2,4);
        audio->samples.setSample(0,0,0.1f);audio->samples.setSample(1,0,-0.1f);
        referencelab::ReferencePlayer player;player.prepare(48000.0);player.setAudio(audio);player.play();
        juce::AudioBuffer<float> rendered(2,1);player.process(rendered);
        expectWithinAbsoluteError(rendered.getSample(0,0),0.1f,0.0001f);
        expectWithinAbsoluteError(rendered.getSample(1,0),-0.1f,0.0001f);
    }
};

ReferenceModelTests referenceModelTests;
}

int main() {
    juce::UnitTestRunner runner;
    runner.runAllTests();
    return runner.getNumResults() > 0 && runner.getResult(0)->failures == 0 ? 0 : 1;
}
