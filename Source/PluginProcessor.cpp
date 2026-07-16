#include "PluginProcessor.h"
#include "PluginEditor.h"
#include <cmath>

juce::AudioProcessorValueTreeState::ParameterLayout ReferenceLabAudioProcessor::createLayout(){
    std::vector<std::unique_ptr<juce::RangedAudioParameter>> p;
    p.push_back(std::make_unique<juce::AudioParameterFloat>("fade","A/B Fade",0.f,100.f,20.f));
    p.push_back(std::make_unique<juce::AudioParameterBool>("safe","Safe Export",true));
    p.push_back(std::make_unique<juce::AudioParameterBool>("eqBypass","EQ Bypass",true));
    p.push_back(std::make_unique<juce::AudioParameterFloat>("hpf","High Pass",juce::NormalisableRange<float>(20.f,2000.f,1.f,.35f),20.f));
    p.push_back(std::make_unique<juce::AudioParameterFloat>("bellFreq","Bell Frequency",juce::NormalisableRange<float>(20.f,20000.f,1.f,.25f),1000.f));
    p.push_back(std::make_unique<juce::AudioParameterFloat>("bellGain","Bell Gain",-18.f,18.f,0.f));
    p.push_back(std::make_unique<juce::AudioParameterFloat>("bellQ","Bell Q",juce::NormalisableRange<float>(.1f,12.f,.01f,.4f),1.f));
    p.push_back(std::make_unique<juce::AudioParameterFloat>("lpf","Low Pass",juce::NormalisableRange<float>(100.f,20000.f,1.f,.25f),20000.f));
    p.push_back(std::make_unique<juce::AudioParameterBool>("hpfEnabled","High Pass Enabled",true));
    p.push_back(std::make_unique<juce::AudioParameterBool>("bellEnabled","Bell Enabled",true));
    p.push_back(std::make_unique<juce::AudioParameterBool>("lpfEnabled","Low Pass Enabled",true));
    p.push_back(std::make_unique<juce::AudioParameterChoice>("hpfSlope","High Pass Slope",juce::StringArray{"12 dB/oct","24 dB/oct"},0));
    p.push_back(std::make_unique<juce::AudioParameterChoice>("lpfSlope","Low Pass Slope",juce::StringArray{"12 dB/oct","24 dB/oct"},0));
    p.push_back(std::make_unique<juce::AudioParameterChoice>("mode","Listening Mode",juce::StringArray{"Stereo","Mono","Mid","Side"},0));
    p.push_back(std::make_unique<juce::AudioParameterBool>("autoMatch","Auto Match",false));
    p.push_back(std::make_unique<juce::AudioParameterChoice>("matchMode","Match Mode",juce::StringArray{"Integrated","Short-Term","Momentary"},0));
    p.push_back(std::make_unique<juce::AudioParameterFloat>("manualGain","Manual Match Gain",-12.f,12.f,0.f));
    p.push_back(std::make_unique<juce::AudioParameterFloat>("matchLimit","Auto Match Limit",1.f,18.f,12.f));
    p.push_back(std::make_unique<juce::AudioParameterBool>("matchBypass","Match Bypass",false));
    p.push_back(std::make_unique<juce::AudioParameterBool>("transportSync","Transport Sync",false));
    p.push_back(std::make_unique<juce::AudioParameterFloat>("syncOffset","Sync Offset",-600.f,600.f,0.f));
    p.push_back(std::make_unique<juce::AudioParameterChoice>("fftOrder","FFT Size",juce::StringArray{"1024","2048","4096"},1));
    p.push_back(std::make_unique<juce::AudioParameterFloat>("fftSmoothing","FFT Smoothing",.01f,1.f,.22f));
    p.push_back(std::make_unique<juce::AudioParameterChoice>("displayRate","Display Rate",juce::StringArray{"15 Hz","30 Hz","60 Hz"},1));
    return {p.begin(),p.end()};
}
ReferenceLabAudioProcessor::ReferenceLabAudioProcessor():AudioProcessor(BusesProperties().withInput("Input",juce::AudioChannelSet::stereo(),true).withOutput("Output",juce::AudioChannelSet::stereo(),true)),state(*this,nullptr,"STATE",createLayout()),manager(juce::File::getSpecialLocation(juce::File::userApplicationDataDirectory).getChildFile("ReferenceLab").getChildFile("reference.json")){formats.registerBasicFormats();manager.load();}
ReferenceLabAudioProcessor::~ReferenceLabAudioProcessor()=default;
void ReferenceLabAudioProcessor::prepareToPlay(double sr,int n){player.prepare(sr);comparison.prepare(sr,n);mixAnalysis.prepare(sr);referenceAnalysis.prepare(sr);outputAnalysis.prepare(sr);matcher.prepare(sr);referenceBuffer.setSize(2,n);blendCurrent=blendTarget=reference.load()?1.f:0.f;blendStep=0.f;blendRemaining=0;}
bool ReferenceLabAudioProcessor::isBusesLayoutSupported(const BusesLayout&l)const{return l.getMainInputChannelSet()==juce::AudioChannelSet::stereo()&&l.getMainOutputChannelSet()==juce::AudioChannelSet::stereo();}
void ReferenceLabAudioProcessor::processBlock(juce::AudioBuffer<float>&mix,juce::MidiBuffer&){
    juce::ScopedNoDenormals noDenormals; if(isNonRealtime()&&state.getRawParameterValue("safe")->load()>.5f)return;
    if(state.getRawParameterValue("transportSync")->load()>.5f){if(auto*head=getPlayHead()){if(auto position=head->getPosition()){transportAvailable.store(true);auto playing=position->getIsPlaying();if(playing){auto seconds=position->getTimeInSeconds().orFallback(0.0)+(double)state.getRawParameterValue("syncOffset")->load();if(std::abs(player.getPositionSeconds()-seconds)>.1)player.seek(juce::jmax(0.0,seconds));player.play();}else player.pause();}else transportAvailable.store(false);}else transportAvailable.store(false);}
    referenceBuffer.setSize(mix.getNumChannels(),mix.getNumSamples(),false,false,true); player.process(referenceBuffer);mixFifo.push(mix);referenceFifo.push(referenceBuffer);mixAnalysis.process(mix);referenceAnalysis.process(referenceBuffer);auto mixMeters=mixAnalysis.snapshot(),refMeters=referenceAnalysis.snapshot();
    referencelab::ComparisonSettings s;s.bypass=state.getRawParameterValue("eqBypass")->load()>.5f;s.highPassEnabled=state.getRawParameterValue("hpfEnabled")->load()>.5f;s.bellEnabled=state.getRawParameterValue("bellEnabled")->load()>.5f;s.lowPassEnabled=state.getRawParameterValue("lpfEnabled")->load()>.5f;s.highPassSlope=12+12*(int)state.getRawParameterValue("hpfSlope")->load();s.lowPassSlope=12+12*(int)state.getRawParameterValue("lpfSlope")->load();s.highPassHz=state.getRawParameterValue("hpf")->load();s.bellHz=state.getRawParameterValue("bellFreq")->load();s.bellGainDb=state.getRawParameterValue("bellGain")->load();s.bellQ=state.getRawParameterValue("bellQ")->load();s.lowPassHz=state.getRawParameterValue("lpf")->load();s.mode=static_cast<referencelab::ListeningMode>((int)state.getRawParameterValue("mode")->load());comparison.update(s);comparison.process(mix,referenceBuffer);auto matchBypassed=state.getRawParameterValue("matchBypass")->load()>.5f;matcher.process(referenceBuffer,mixMeters,refMeters,!matchBypassed&&state.getRawParameterValue("autoMatch")->load()>.5f,static_cast<referencelab::LoudnessMode>((int)state.getRawParameterValue("matchMode")->load()),matchBypassed?0.f:state.getRawParameterValue("manualGain")->load(),state.getRawParameterValue("matchLimit")->load());
    auto requested=reference.load()&&player.isLoaded()?1.f:0.f;if(requested!=blendTarget){blendTarget=requested;blendRemaining=juce::jmax(1,(int)(getSampleRate()*state.getRawParameterValue("fade")->load()/1000.f));blendStep=(blendTarget-blendCurrent)/(float)blendRemaining;}
    for(int i=0;i<mix.getNumSamples();++i){if(blendRemaining>0){blendCurrent+=blendStep;if(--blendRemaining==0)blendCurrent=blendTarget;}auto a=blendCurrent;for(int c=0;c<mix.getNumChannels();++c)mix.setSample(c,i,mix.getSample(c,i)*(1.f-a)+referenceBuffer.getSample(c,i)*a);}outputAnalysis.process(mix);outputFifo.push(mix);
}
bool ReferenceLabAudioProcessor::loadFile(const juce::File&f,juce::String&e){auto ok=player.load(f,formats,e);if(ok){juce::String catalogError;manager.addFile(f,catalogError);player.play();}return ok;}
void ReferenceLabAudioProcessor::loadFileAsync(const juce::File&f,std::function<void(const juce::String&)>done){if(!f.existsAsFile()){if(done)done("File non trovato");return;}auto library=manager.snapshot();for(auto&m:library.references)if(m.resolveAgainst(library.root)==f){player.setStartOffset(m.startOffsetSeconds);player.setLoop(m.loop.enabled,m.loop.startSeconds,m.loop.endSeconds,m.loop.crossfadeMs);break;}cache.loadAsync(f,[this,f,done=std::move(done)](std::shared_ptr<referencelab::ReferenceAudioData>data,const juce::String&error,bool){if(data){player.setAudio(std::move(data));if(auto restore=pendingRestorePosition.exchange(-1.0);restore>=0.0)player.seek(restore);player.play();{const juce::ScopedLock lock(activeFileLock);activeFile=f;}juce::String catalogError;manager.addFile(f,catalogError);}if(done)done(error);});}
bool ReferenceLabAudioProcessor::saveComparisonPreset(const juce::File&file,juce::String&error){file.getParentDirectory().createDirectory();auto tree=state.copyState();for(auto property:{juce::Identifier("reference"),juce::Identifier("referencePosition"),juce::Identifier("activeReferencePath")})tree.removeProperty(property,nullptr);auto xml=tree.createXml();if(!xml||!file.replaceWithText(xml->toString())){error="Impossibile salvare il preset";return false;}return true;}
bool ReferenceLabAudioProcessor::loadComparisonPreset(const juce::File&file,juce::String&error){auto xml=juce::XmlDocument::parse(file);if(!xml){error="Preset non valido";return false;}auto tree=juce::ValueTree::fromXml(*xml);if(!tree.isValid()||tree.getType()!=state.state.getType()){error="Formato preset non compatibile";return false;}for(auto property:{juce::Identifier("reference"),juce::Identifier("referencePosition"),juce::Identifier("activeReferencePath")})tree.removeProperty(property,nullptr);state.replaceState(tree);return true;}
void ReferenceLabAudioProcessor::getStateInformation(juce::MemoryBlock&m){auto v=state.copyState();v.setProperty("reference",reference.load(),nullptr);v.setProperty("referencePosition",player.getPositionSeconds(),nullptr);{const juce::ScopedLock lock(activeFileLock);v.setProperty("activeReferencePath",activeFile.getFullPathName(),nullptr);}if(auto xml=v.createXml())copyXmlToBinary(*xml,m);}
void ReferenceLabAudioProcessor::setStateInformation(const void*d,int n){if(auto xml=getXmlFromBinary(d,n)){auto v=juce::ValueTree::fromXml(*xml);if(v.isValid()){reference.store((bool)v.getProperty("reference"));auto path=v.getProperty("activeReferencePath").toString();pendingRestorePosition.store(path.isNotEmpty()?(double)v.getProperty("referencePosition",0.0):-1.0);state.replaceState(v);if(path.isNotEmpty())loadFileAsync(juce::File(path),{});}}}
juce::AudioProcessorEditor* ReferenceLabAudioProcessor::createEditor(){return new ReferenceLabEditor(*this);} juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter(){return new ReferenceLabAudioProcessor();}
