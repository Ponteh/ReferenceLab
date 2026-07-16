#include "PluginProcessor.h"
#include "PluginEditor.h"

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
    p.push_back(std::make_unique<juce::AudioParameterChoice>("mode","Listening Mode",juce::StringArray{"Stereo","Mono","Mid","Side"},0));
    p.push_back(std::make_unique<juce::AudioParameterBool>("autoMatch","Auto Match",false));
    p.push_back(std::make_unique<juce::AudioParameterChoice>("matchMode","Match Mode",juce::StringArray{"Integrated","Short-Term","Momentary"},0));
    p.push_back(std::make_unique<juce::AudioParameterFloat>("manualGain","Manual Match Gain",-12.f,12.f,0.f));
    p.push_back(std::make_unique<juce::AudioParameterFloat>("matchLimit","Auto Match Limit",1.f,18.f,12.f));
    return {p.begin(),p.end()};
}
ReferenceLabAudioProcessor::ReferenceLabAudioProcessor():AudioProcessor(BusesProperties().withInput("Input",juce::AudioChannelSet::stereo(),true).withOutput("Output",juce::AudioChannelSet::stereo(),true)),state(*this,nullptr,"STATE",createLayout()),manager(juce::File::getSpecialLocation(juce::File::userApplicationDataDirectory).getChildFile("ReferenceLab").getChildFile("reference.json")){formats.registerBasicFormats();manager.load();}
ReferenceLabAudioProcessor::~ReferenceLabAudioProcessor()=default;
void ReferenceLabAudioProcessor::prepareToPlay(double sr,int n){player.prepare(sr);comparison.prepare(sr,n);mixAnalysis.prepare(sr);referenceAnalysis.prepare(sr);outputAnalysis.prepare(sr);matcher.prepare(sr);referenceBuffer.setSize(2,n);blend.reset(sr,.02);blend.setCurrentAndTargetValue(reference.load()?1.f:0.f);}
bool ReferenceLabAudioProcessor::isBusesLayoutSupported(const BusesLayout&l)const{return l.getMainInputChannelSet()==juce::AudioChannelSet::stereo()&&l.getMainOutputChannelSet()==juce::AudioChannelSet::stereo();}
void ReferenceLabAudioProcessor::processBlock(juce::AudioBuffer<float>&mix,juce::MidiBuffer&){
    juce::ScopedNoDenormals noDenormals; if(isNonRealtime()&&state.getRawParameterValue("safe")->load()>.5f)return;
    referenceBuffer.setSize(mix.getNumChannels(),mix.getNumSamples(),false,false,true); player.process(referenceBuffer);mixFifo.push(mix);referenceFifo.push(referenceBuffer);mixAnalysis.process(mix);referenceAnalysis.process(referenceBuffer);auto mixMeters=mixAnalysis.snapshot(),refMeters=referenceAnalysis.snapshot();
    referencelab::ComparisonSettings s; s.bypass=state.getRawParameterValue("eqBypass")->load()>.5f;s.highPassHz=state.getRawParameterValue("hpf")->load();s.bellHz=state.getRawParameterValue("bellFreq")->load();s.bellGainDb=state.getRawParameterValue("bellGain")->load();s.bellQ=state.getRawParameterValue("bellQ")->load();s.lowPassHz=state.getRawParameterValue("lpf")->load();s.mode=static_cast<referencelab::ListeningMode>((int)state.getRawParameterValue("mode")->load());comparison.update(s);comparison.process(mix,referenceBuffer);matcher.process(referenceBuffer,mixMeters,refMeters,state.getRawParameterValue("autoMatch")->load()>.5f,static_cast<referencelab::LoudnessMode>((int)state.getRawParameterValue("matchMode")->load()),state.getRawParameterValue("manualGain")->load(),state.getRawParameterValue("matchLimit")->load());
    blend.setTargetValue(reference.load()&&player.isLoaded()?1.f:0.f);
    for(int i=0;i<mix.getNumSamples();++i){auto a=blend.getNextValue();for(int c=0;c<mix.getNumChannels();++c)mix.setSample(c,i,mix.getSample(c,i)*(1.f-a)+referenceBuffer.getSample(c,i)*a);}outputAnalysis.process(mix);outputFifo.push(mix);
}
bool ReferenceLabAudioProcessor::loadFile(const juce::File&f,juce::String&e){auto ok=player.load(f,formats,e);if(ok){juce::String catalogError;manager.addFile(f,catalogError);player.play();}return ok;}
void ReferenceLabAudioProcessor::loadFileAsync(const juce::File&f,std::function<void(const juce::String&)>done){if(!f.existsAsFile()){if(done)done("File non trovato");return;}cache.loadAsync(f,[this,f,done=std::move(done)](std::shared_ptr<referencelab::ReferenceAudioData>data,const juce::String&error,bool){if(data){player.setAudio(std::move(data));player.play();juce::String catalogError;manager.addFile(f,catalogError);}if(done)done(error);});}
void ReferenceLabAudioProcessor::getStateInformation(juce::MemoryBlock&m){auto v=state.copyState();v.setProperty("reference",reference.load(),nullptr);if(auto xml=v.createXml())copyXmlToBinary(*xml,m);}
void ReferenceLabAudioProcessor::setStateInformation(const void*d,int n){if(auto xml=getXmlFromBinary(d,n)){auto v=juce::ValueTree::fromXml(*xml);if(v.isValid()){reference.store((bool)v.getProperty("reference"));state.replaceState(v);}}}
juce::AudioProcessorEditor* ReferenceLabAudioProcessor::createEditor(){return new ReferenceLabEditor(*this);} juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter(){return new ReferenceLabAudioProcessor();}
