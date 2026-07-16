#include <juce_core/juce_core.h>
#include "Library/ReferenceModel.h"
#include "Audio/ComparisonProcessor.h"
#include "Audio/ReferencePlayer.h"
#include "Audio/AnalysisEngine.h"
#include "Audio/SampleFifo.h"

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

        beginTest("Reference player wraps an A/B loop");
        audio->sampleRate=4.0;for(int i=0;i<4;++i){audio->samples.setSample(0,i,(float)i);audio->samples.setSample(1,i,(float)i);}player.prepare(4.0);player.setAudio(audio);player.setLoop(true,.25,.75,0.0);player.seek(.5);player.play();juce::AudioBuffer<float> looped(2,2);player.process(looped);
        expectWithinAbsoluteError(looped.getSample(0,0),2.f,.0001f);expectWithinAbsoluteError(looped.getSample(0,1),1.f,.0001f);

        beginTest("Analysis metrics for correlated stereo");
        juce::AudioBuffer<float> meterSignal(2,4800);meterSignal.clear();
        for(int i=0;i<meterSignal.getNumSamples();++i){meterSignal.setSample(0,i,0.5f);meterSignal.setSample(1,i,0.5f);}
        referencelab::AnalysisEngine analyser;analyser.prepare(48000.0);analyser.process(meterSignal);auto meters=analyser.snapshot();
        expectWithinAbsoluteError(meters.peakDb,-6.0206f,0.02f);
        expectWithinAbsoluteError(meters.rmsDb,-6.0206f,0.02f);
        expectWithinAbsoluteError(meters.correlation,1.0f,0.001f);
        expectWithinAbsoluteError(meters.stereoWidth,0.0f,0.001f);

        beginTest("Lock-free sample FIFO preserves mono sum");
        referencelab::SampleFifo fifo;juce::AudioBuffer<float> fifoInput(2,2);fifoInput.setSample(0,0,1.f);fifoInput.setSample(1,0,-1.f);fifoInput.setSample(0,1,.5f);fifoInput.setSample(1,1,.5f);fifo.push(fifoInput);float fifoOutput[2]{};
        expectEquals(fifo.pull(fifoOutput,2),2);expectWithinAbsoluteError(fifoOutput[0],0.f,.0001f);expectWithinAbsoluteError(fifoOutput[1],.5f,.0001f);
    }
};

ReferenceModelTests referenceModelTests;
}

int main() {
    juce::UnitTestRunner runner;
    runner.runAllTests();
    return runner.getNumResults() > 0 && runner.getResult(0)->failures == 0 ? 0 : 1;
}
