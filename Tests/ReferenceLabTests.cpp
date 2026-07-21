#include <juce_core/juce_core.h>
#include "Library/ReferenceModel.h"
#include "Library/MetadataRepository.h"
#include "Audio/ComparisonProcessor.h"
#include "Audio/ReferencePlayer.h"
#include "Audio/AnalysisEngine.h"
#include "Audio/SampleFifo.h"
#include "Audio/SpectrumAnalyzer.h"
#include "Audio/TransportController.h"
#include "Audio/CacheManager.h"
#include "Library/JsonCatalogProvider.h"
#include "Library/HttpReferenceProvider.h"
#include "Library/ReferenceManager.h"
#include <algorithm>
#include <cmath>

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

        beginTest("Swap L/R is identical and smoothed for Mix and Reference");
        referencelab::ComparisonProcessor swapProcessor;swapProcessor.prepare(48000.0,600);referencelab::ComparisonSettings swapSettings;swapSettings.swapLeftRight=true;swapProcessor.update(swapSettings);juce::AudioBuffer<float>swapMix(2,600),swapReference(2,600);for(int i=0;i<600;++i){swapMix.setSample(0,i,.8f);swapMix.setSample(1,i,-.2f);swapReference.setSample(0,i,.4f);swapReference.setSample(1,i,-.6f);}swapProcessor.process(swapMix,swapReference);expectWithinAbsoluteError(swapMix.getSample(0,599),-.2f,.001f);expectWithinAbsoluteError(swapMix.getSample(1,599),.8f,.001f);expectWithinAbsoluteError(swapReference.getSample(0,599),-.6f,.001f);expectWithinAbsoluteError(swapReference.getSample(1,599),.4f,.001f);

        beginTest("Listen mode is isolated from the Compare signal");
        referencelab::ComparisonProcessor monitorProcessor;monitorProcessor.prepare(48000.0,600);referencelab::ComparisonSettings monitorSettings;monitorSettings.bypass=true;monitorSettings.mode=referencelab::ListeningMode::side;monitorProcessor.update(monitorSettings);juce::AudioBuffer<float>compareMix(2,600),compareReference(2,600);for(int i=0;i<600;++i){compareMix.setSample(0,i,.8f);compareMix.setSample(1,i,.2f);compareReference.setSample(0,i,.4f);compareReference.setSample(1,i,.1f);}monitorProcessor.processEqualizer(compareMix,compareReference);expectWithinAbsoluteError(compareMix.getSample(0,599),.8f,.0001f);expectWithinAbsoluteError(compareMix.getSample(1,599),.2f,.0001f);monitorProcessor.processListeningMode(compareMix);expectWithinAbsoluteError(compareMix.getSample(0,599),.3f,.001f);expectWithinAbsoluteError(compareMix.getSample(1,599),-.3f,.001f);

        beginTest("Reference player renders cached audio");
        auto audio=std::make_shared<referencelab::ReferenceAudioData>();
        audio->sampleRate=48000.0;audio->samples.setSize(2,4);
        audio->samples.setSample(0,0,0.1f);audio->samples.setSample(1,0,-0.1f);
        referencelab::ReferencePlayer player;player.prepare(48000.0);player.setAudio(audio);player.play();
        juce::AudioBuffer<float> rendered(2,1);player.process(rendered);
        expectWithinAbsoluteError(rendered.getSample(0,0),0.1f,0.0001f);
        expectWithinAbsoluteError(rendered.getSample(1,0),-0.1f,0.0001f);

        beginTest("Cache limit is configurable and bounded");
        referencelab::CacheManager cache(1);expect(cache.getMaximumBytes()>=16u*1024u*1024u);cache.setMaximumBytes(128u*1024u*1024u);expectEquals(cache.getMaximumBytes(),(std::size_t)(128u*1024u*1024u));cache.clear();expectEquals(cache.getCurrentBytes(),(std::size_t)0);

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

        beginTest("K-weighted gated integrated loudness");
        referencelab::AnalysisEngine loudness; loudness.prepare(48000.0);
        juce::AudioBuffer<float> tone(2,480);double phase=0.0;
        for(int block=0;block<400;++block){for(int i=0;i<tone.getNumSamples();++i){auto sample=.1f*(float)std::sin(phase);phase+=2.0*juce::MathConstants<double>::pi*1000.0/48000.0;tone.setSample(0,i,sample);tone.setSample(1,i,sample);}loudness.process(tone);}
        auto loudnessResult=loudness.snapshot();expectWithinAbsoluteError(loudnessResult.integratedLufs,-20.0f,1.5f);

        beginTest("Comparison bypass remains transparent");
        referencelab::ComparisonProcessor transparent;transparent.prepare(48000.0,16);referencelab::ComparisonSettings transparentSettings;transparentSettings.bypass=true;transparent.update(transparentSettings);juce::AudioBuffer<float> transparentMix(2,16),transparentRef(2,16);for(int i=0;i<16;++i){transparentMix.setSample(0,i,(float)i/16.f);transparentMix.setSample(1,i,-(float)i/16.f);transparentRef.setSample(0,i,.25f);transparentRef.setSample(1,i,-.25f);}juce::AudioBuffer<float> expectedMix(transparentMix),expectedRef(transparentRef);transparent.process(transparentMix,transparentRef);for(int c=0;c<2;++c)for(int i=0;i<16;++i){expectWithinAbsoluteError(transparentMix.getSample(c,i),expectedMix.getSample(c,i),.000001f);expectWithinAbsoluteError(transparentRef.getSample(c,i),expectedRef.getSample(c,i),.000001f);}

        beginTest("Band-pass favours its centre frequency");
        auto bandPassLevel=[](double frequency){referencelab::ComparisonProcessor processor;processor.prepare(48000.0,48000);referencelab::ComparisonSettings settings;settings.bypass=false;settings.highPassEnabled=false;settings.lowPassEnabled=false;settings.bandPassEnabled=true;settings.bandPassHz=1000.f;settings.bandPassQ=4.f;processor.update(settings);juce::AudioBuffer<float> mix(2,48000),reference(2,48000);for(int i=0;i<48000;++i){auto sample=(float)std::sin(2.0*juce::MathConstants<double>::pi*frequency*i/48000.0);mix.setSample(0,i,sample);mix.setSample(1,i,sample);reference.setSample(0,i,sample);reference.setSample(1,i,sample);}processor.process(mix,reference);return mix.getRMSLevel(0,24000,24000);};
        expect(bandPassLevel(1000.0)>bandPassLevel(100.0)*5.f);
        referencelab::ComparisonSettings responseSettings;responseSettings.bypass=false;responseSettings.highPassEnabled=false;responseSettings.lowPassEnabled=false;responseSettings.bandPassEnabled=true;responseSettings.bandPassHz=1000.f;responseSettings.bandPassQ=4.f;expect(referencelab::ComparisonProcessor::responseDecibelsAt(responseSettings,1000.f,48000.0)>referencelab::ComparisonProcessor::responseDecibelsAt(responseSettings,100.f,48000.0)+20.f);

        beginTest("Lock-free sample FIFO preserves mono sum");
        referencelab::SampleFifo fifo;juce::AudioBuffer<float> fifoInput(2,2);fifoInput.setSample(0,0,1.f);fifoInput.setSample(1,0,-1.f);fifoInput.setSample(0,1,.5f);fifoInput.setSample(1,1,.5f);fifo.push(fifoInput);float fifoOutput[2]{};
        expectEquals(fifo.pull(fifoOutput,2),2);expectWithinAbsoluteError(fifoOutput[0],0.f,.0001f);expectWithinAbsoluteError(fifoOutput[1],.5f,.0001f);referencelab::SampleFifo sideFifo;sideFifo.push(fifoInput,referencelab::SampleChannelView::side);expectEquals(sideFifo.pull(fifoOutput,2),2);expectWithinAbsoluteError(fifoOutput[0],1.f,.0001f);expectWithinAbsoluteError(fifoOutput[1],0.f,.0001f);

        beginTest("Shared spectrum analyzer locates a sine wave");
        referencelab::SampleFifo spectrumFifo;juce::AudioBuffer<float> spectrumInput(2,2048);for(int i=0;i<spectrumInput.getNumSamples();++i){auto sample=(float)std::sin(2.0*juce::MathConstants<double>::pi*1000.0*i/48000.0);spectrumInput.setSample(0,i,sample);spectrumInput.setSample(1,i,sample);}spectrumFifo.push(spectrumInput);referencelab::SpectrumAnalyzer spectrumAnalyzer(11);expect(spectrumAnalyzer.update(spectrumFifo));auto&bins=spectrumAnalyzer.getSpectrum();auto peak=(int)std::distance(bins.begin(),std::max_element(bins.begin()+1,bins.end()));expect(peak>=41&&peak<=44,"Expected the FFT peak near the 1 kHz bin");

        beginTest("Spectrum analyzer supports FFT 32768");
        referencelab::SpectrumAnalyzer extendedSpectrum(15);expectEquals(extendedSpectrum.getFftSize(),32768);

        beginTest("Metadata repository saves atomically and reloads Unicode");
        auto testDirectory=juce::File::getSpecialLocation(juce::File::tempDirectory).getNonexistentChildFile("ReferenceLabTests",{},false);testDirectory.createDirectory();referencelab::MetadataRepository repository(testDirectory.getChildFile("reference.json"));referencelab::ReferenceLibrary library;library.root=testDirectory;referencelab::ReferenceMetadata stored;stored.uuid="repository-test";stored.title=juce::String::fromUTF8("Traccia è");stored.source="track.wav";library.references.push_back(stored);juce::String repositoryError;expect(repository.save(library,repositoryError),repositoryError);auto loaded=repository.load(repositoryError);expectEquals((int)loaded.references.size(),1);if(!loaded.references.empty())expectEquals(loaded.references.front().title,stored.title);expect(repository.save(library,repositoryError),repositoryError);expect(testDirectory.getChildFile("reference.json.bak").existsAsFile());testDirectory.deleteRecursively();
        beginTest("Manager imports JSON catalogs and remote URLs");auto v2Directory=juce::File::getSpecialLocation(juce::File::tempDirectory).getNonexistentChildFile("ReferenceLabV2Tests",{},false);v2Directory.createDirectory();referencelab::ReferenceMetadata catalogItem;catalogItem.uuid="catalog-item";catalogItem.title="Catalog Track";catalogItem.source="audio/track.wav";auto*catalogRoot=new juce::DynamicObject();juce::Array<juce::var>catalogEntries;catalogEntries.add(catalogItem.toVar());catalogRoot->setProperty("references",catalogEntries);auto catalogFile=v2Directory.getChildFile("catalog.json");expect(catalogFile.replaceWithText(juce::JSON::toString(juce::var(catalogRoot))));referencelab::ReferenceManager v2Manager(v2Directory.getChildFile("library.json"));juce::String urlError;expectEquals(v2Manager.importCatalog(catalogFile,urlError),1);expect(v2Manager.addUrl("https://example.com/reference.flac",urlError),urlError);auto v2Snapshot=v2Manager.snapshot();expectEquals((int)v2Snapshot.references.size(),2);if(!v2Snapshot.references.empty())expect(juce::File::isAbsolutePath(v2Snapshot.references.front().source));v2Directory.deleteRecursively();
        beginTest("HTTP provider validates URLs");referencelab::HttpReferenceProvider http;expect(http.supportsUrl(juce::URL("https://example.com/reference.flac")));expect(!http.supportsUrl(juce::URL("ftp://example.com/reference.wav")));auto remote=http.inspectUrl(juce::URL("https://example.com/reference.wav"),urlError);expect(remote.has_value(),urlError);
        beginTest("Library filters rating and sorts album or year");auto sortingRoot=juce::File::getSpecialLocation(juce::File::tempDirectory).getNonexistentChildFile("ReferenceLabSortingTests",{},false);sortingRoot.createDirectory();referencelab::ReferenceManager sortingManager(sortingRoot.getChildFile("reference.json"));sortingManager.load();juce::String sortingError;expect(sortingManager.addUrl("https://example.com/older.wav",sortingError),sortingError);expect(sortingManager.addUrl("https://example.com/newer.wav",sortingError),sortingError);auto sortingItems=sortingManager.snapshot().references;if(sortingItems.size()==2){sortingItems[0].album="Zulu";sortingItems[0].year=2020;sortingItems[0].rating=2;sortingItems[1].album="Alpha";sortingItems[1].year=2024;sortingItems[1].rating=5;expect(sortingManager.updateMetadata(sortingItems[0],sortingError),sortingError);expect(sortingManager.updateMetadata(sortingItems[1],sortingError),sortingError);referencelab::ReferenceFilter ratingFilter;ratingFilter.minimumRating=4;expectEquals((int)sortingManager.filter(ratingFilter).size(),1);referencelab::ReferenceFilter albumSort;albumSort.sort=referencelab::ReferenceSort::album;auto byAlbum=sortingManager.filter(albumSort);expect(!byAlbum.empty());if(!byAlbum.empty())expectEquals(byAlbum.front().album,juce::String("Alpha"));referencelab::ReferenceFilter yearSort;yearSort.sort=referencelab::ReferenceSort::year;yearSort.ascending=false;auto byYear=sortingManager.filter(yearSort);expect(!byYear.empty());if(!byYear.empty())expectEquals(byYear.front().year.value_or(0),2024);}sortingRoot.deleteRecursively();
        beginTest("Playlist cycles and round trips JSON");referencelab::ReferencePlaylist playlist;playlist.name="Mastering";expect(playlist.add("a"));expect(playlist.add("b"));expectEquals(playlist.selectRelative(1),juce::String("b"));auto restored=referencelab::ReferencePlaylist::fromVar(playlist.toVar(),urlError);expect(restored.has_value(),urlError);if(restored)expectEquals(restored->referenceIds.size(),2);
        beginTest("Manager persists playlists and quick selection");auto playlistDirectory=juce::File::getSpecialLocation(juce::File::tempDirectory).getNonexistentChildFile("ReferenceLabPlaylistTests",{},false);playlistDirectory.createDirectory();referencelab::ReferenceManager playlistManager(playlistDirectory.getChildFile("reference.json"));expect(playlistManager.addUrl("https://example.com/one.mp3",urlError),urlError);expect(playlistManager.createPlaylist("A/B",urlError),urlError);auto playlistLibrary=playlistManager.snapshot();auto savedPlaylists=playlistManager.playlistsSnapshot();expectEquals((int)savedPlaylists.size(),1);if(!savedPlaylists.empty()&&!playlistLibrary.references.empty()){expect(playlistManager.addToPlaylist(savedPlaylists[0].id,playlistLibrary.references[0].uuid,urlError),urlError);auto selected=playlistManager.selectPlaylistRelative(savedPlaylists[0].id,1,urlError);expect(selected.has_value(),urlError);}referencelab::ReferenceManager reloadedPlaylists(playlistDirectory.getChildFile("reference.json"));reloadedPlaylists.load();expectEquals((int)reloadedPlaylists.playlistsSnapshot().size(),1);playlistDirectory.deleteRecursively();
        beginTest("Advanced transport modes");referencelab::TransportController transport;auto timeline=transport.update(125.0,true,60.0,0.0,referencelab::TransportSyncMode::timeline);expect(timeline.available&&timeline.seek);expectWithinAbsoluteError(timeline.positionSeconds,5.0,0.0001);transport.reset();auto stoppedTransport=transport.update(10.0,false,60.0,3.0,referencelab::TransportSyncMode::restartOnPlay);expect(stoppedTransport.pause&&!stoppedTransport.seek);auto startedTransport=transport.update(10.1,true,60.0,3.0,referencelab::TransportSyncMode::restartOnPlay);expect(startedTransport.play&&startedTransport.seek);expectWithinAbsoluteError(startedTransport.positionSeconds,3.0,0.0001);auto unavailable=transport.update(std::nullopt,true,60.0,0.0,referencelab::TransportSyncMode::timeline);expect(!unavailable.available);
        beginTest("Multiple libraries remain isolated and persist selection");auto multiRoot=juce::File::getSpecialLocation(juce::File::tempDirectory).getNonexistentChildFile("ReferenceLabMultiLibraryTests",{},false);multiRoot.createDirectory();referencelab::ReferenceManager multiManager(multiRoot.getChildFile("reference.json"));multiManager.load();auto mainLibraryId=multiManager.activeLibraryId();juce::String multiError;expect(multiManager.createLibrary("Seconda",multiRoot.getChildFile("Seconda"),multiError),multiError);expectEquals((int)multiManager.librariesSnapshot().size(),2);expect(multiManager.addUrl("https://example.com/second.wav",multiError),multiError);expectEquals((int)multiManager.snapshot().references.size(),1);expect(multiManager.switchLibrary(mainLibraryId,multiError),multiError);expectEquals((int)multiManager.snapshot().references.size(),0);referencelab::ReferenceManager multiReloaded(multiRoot.getChildFile("reference.json"));multiReloaded.load();expectEquals((int)multiReloaded.librariesSnapshot().size(),2);expectEquals(multiReloaded.activeLibraryId(),mainLibraryId);multiRoot.deleteRecursively();
        beginTest("Remote JSON catalogs resolve relative audio URLs");auto remoteCatalogRoot=juce::File::getSpecialLocation(juce::File::tempDirectory).getNonexistentChildFile("ReferenceLabRemoteCatalogTests",{},false);remoteCatalogRoot.createDirectory();referencelab::ReferenceManager remoteCatalogManager(remoteCatalogRoot.getChildFile("reference.json"));referencelab::ReferenceMetadata remoteCatalogItem;remoteCatalogItem.uuid="remote-relative";remoteCatalogItem.title="Remote Relative";remoteCatalogItem.source="audio/reference.mp3";auto*remoteJsonRoot=new juce::DynamicObject();juce::Array<juce::var>remoteJsonItems;remoteJsonItems.add(remoteCatalogItem.toVar());remoteJsonRoot->setProperty("references",remoteJsonItems);juce::String remoteCatalogError;expectEquals(remoteCatalogManager.importRemoteCatalog(juce::JSON::toString(juce::var(remoteJsonRoot)),juce::URL("https://catalog.example.com/library/catalog.json"),remoteCatalogError),1);auto importedRemoteCatalog=remoteCatalogManager.snapshot();if(!importedRemoteCatalog.references.empty())expectEquals(importedRemoteCatalog.references[0].source,juce::String("https://catalog.example.com/library/audio/reference.mp3"));remoteCatalogRoot.deleteRecursively();
        beginTest("Playlist and library lifecycle operations");auto lifecycleRoot=juce::File::getSpecialLocation(juce::File::tempDirectory).getNonexistentChildFile("ReferenceLabLifecycleTests",{},false);lifecycleRoot.createDirectory();referencelab::ReferenceManager lifecycle(lifecycleRoot.getChildFile("reference.json"));lifecycle.load();juce::String lifecycleError;expect(lifecycle.addUrl("https://example.com/a.wav",lifecycleError),lifecycleError);expect(lifecycle.addUrl("https://example.com/b.wav",lifecycleError),lifecycleError);auto lifecycleRefs=lifecycle.snapshot().references;expect(lifecycle.createPlaylist("Original",lifecycleError),lifecycleError);auto lifecyclePlaylist=lifecycle.playlistsSnapshot()[0];expect(lifecycle.addToPlaylist(lifecyclePlaylist.id,lifecycleRefs[0].uuid,lifecycleError),lifecycleError);expect(lifecycle.addToPlaylist(lifecyclePlaylist.id,lifecycleRefs[1].uuid,lifecycleError),lifecycleError);expect(lifecycle.movePlaylistItem(lifecyclePlaylist.id,lifecycleRefs[1].uuid,-1,lifecycleError),lifecycleError);expectEquals(lifecycle.playlistsSnapshot()[0].referenceIds[0],lifecycleRefs[1].uuid);expect(lifecycle.renamePlaylist(lifecyclePlaylist.id,"Rinominata",lifecycleError),lifecycleError);expect(lifecycle.remove(lifecycleRefs[1].uuid,lifecycleError),lifecycleError);expect(!lifecycle.playlistsSnapshot()[0].referenceIds.contains(lifecycleRefs[1].uuid));expect(lifecycle.deletePlaylist(lifecyclePlaylist.id,lifecycleError),lifecycleError);auto lifecycleMain=lifecycle.activeLibraryId();expect(lifecycle.createLibrary("Temporanea",lifecycleRoot.getChildFile("Temporanea"),lifecycleError),lifecycleError);auto temporaryId=lifecycle.activeLibraryId();expect(lifecycle.switchLibrary(lifecycleMain,lifecycleError),lifecycleError);expect(lifecycle.renameLibrary(temporaryId,"Archivio",lifecycleError),lifecycleError);expect(lifecycle.unregisterLibrary(temporaryId,lifecycleError),lifecycleError);expectEquals((int)lifecycle.librariesSnapshot().size(),1);lifecycleRoot.deleteRecursively();
    }
};

ReferenceModelTests referenceModelTests;
}

int main() {
    juce::UnitTestRunner runner;
    runner.runAllTests();
    return runner.getNumResults() > 0 && runner.getResult(0)->failures == 0 ? 0 : 1;
}
