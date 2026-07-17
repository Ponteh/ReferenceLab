#include "GuiMagicTheme.h"

namespace referencelab {
namespace {
juce::ValueTree findById(const juce::ValueTree&node,const juce::String&id)
{
    if(node.getProperty("id").toString()==id)return node;
    for(auto child:node)
        if(auto result=findById(child,id);result.isValid())return result;
    return{};
}
}

void setGuiMagicLibraryRatio(juce::ValueTree&tree,double libraryWidthRatio)
{
    const auto ratio=juce::jlimit(.25,.55,libraryWidthRatio);
    if(auto library=findById(tree,"libraryPane");library.isValid())
        library.setProperty("flex-grow",ratio*100.0,nullptr);
    if(auto content=findById(tree,"contentPane");content.isValid())
        content.setProperty("flex-grow",(1.0-ratio)*100.0,nullptr);
}

juce::ValueTree createGuiMagicTheme(double libraryWidthRatio)
{
    static constexpr auto layout=R"xml(
<magic>
  <Styles>
    <Style name="default">
      <Nodes/>
      <Classes>
        <header background-color="ff18212c" border="1" border-color="ff344455" radius="8" padding="5"/>
        <workspace background-color="ff10151d" margin="8 0 0 0" padding="0"/>
        <panel background-color="ff202b38" border="1" border-color="ff344455" radius="8" padding="10"/>
        <page background-color="ff202b38" flex-direction="column" padding="12"/>
        <row flex-direction="row" flex-grow="0" flex-shrink="0" min-height="38" max-height="42"/>
        <fill flex-grow="1" flex-shrink="1"/>
        <fixed flex-grow="0" flex-shrink="0"/>
      </Classes>
      <Types>
        <View margin="0" padding="0"/>
        <ReferenceLabComponent margin="3" padding="0" border="0"/>
      </Types>
      <Palettes><default/></Palettes>
    </Style>
  </Styles>
  <View id="root" flex-direction="column" background-color="ff10151d" margin="16" padding="0">
    <View id="header" class="header" flex-direction="row" flex-grow="0" flex-shrink="0" min-height="48" max-height="48">
      <ReferenceLabComponent id="title" class="fill" min-width="170"/>
      <ReferenceLabComponent id="librarySelector" class="fixed" min-width="170" max-width="170"/>
      <ReferenceLabComponent id="newLibrary" class="fixed" min-width="105" max-width="105"/>
      <ReferenceLabComponent id="transportStatus" class="fixed" min-width="175" max-width="175"/>
      <ReferenceLabComponent id="mix" class="fixed" min-width="75" max-width="75"/>
      <ReferenceLabComponent id="ref" class="fixed" min-width="105" max-width="105"/>
    </View>
    <View id="workspace" class="workspace" flex-direction="row" flex-grow="1">
      <View id="libraryPane" class="panel" flex-direction="column" min-width="290" max-width="520" flex-grow="34">
        <View class="row">
          <ReferenceLabComponent id="add" class="fill"/>
          <ReferenceLabComponent id="scan" class="fill"/>
        </View>
        <View class="row">
          <ReferenceLabComponent id="importCatalog" class="fill"/>
          <ReferenceLabComponent id="addUrl" class="fill"/>
        </View>
        <View class="row">
          <ReferenceLabComponent id="remove" class="fixed" min-width="72" max-width="72"/>
          <ReferenceLabComponent id="relink" class="fixed" min-width="78" max-width="78"/>
          <ReferenceLabComponent id="recursiveScan" class="fill"/>
        </View>
        <View class="row">
          <ReferenceLabComponent id="search" flex-grow="2"/>
          <ReferenceLabComponent id="favouritesOnly" flex-grow="1"/>
        </View>
        <View class="row">
          <ReferenceLabComponent id="sort" class="fixed" min-width="105" max-width="105"/>
          <ReferenceLabComponent id="sortDescending" class="fixed" min-width="68" max-width="68"/>
          <ReferenceLabComponent id="playlist" class="fill"/>
        </View>
        <View class="row">
          <ReferenceLabComponent id="newPlaylist" class="fixed" min-width="62" max-width="62"/>
          <ReferenceLabComponent id="addToPlaylist" class="fixed" min-width="86" max-width="86"/>
          <View class="fill"/>
          <ReferenceLabComponent id="previousReference" class="fixed" min-width="34" max-width="34"/>
          <ReferenceLabComponent id="nextReference" class="fixed" min-width="34" max-width="34"/>
        </View>
        <ReferenceLabComponent id="list" class="fill" min-height="160"/>
      </View>
      <ReferenceLabComponent id="mainSplitter" class="fixed" min-width="10" max-width="10" margin="0"/>
      <View id="contentPane" class="panel" display="tabbed" tab-height="34" selected-tab="uiTab" min-width="430" flex-grow="66">
        <View id="referencePage" class="page" tab-caption="Reference">
          <ReferenceLabComponent id="waveform" class="fill" min-height="190"/>
          <View class="row">
            <ReferenceLabComponent id="play" class="fixed" min-width="68" max-width="68"/>
            <ReferenceLabComponent id="pause" class="fixed" min-width="72" max-width="72"/>
            <ReferenceLabComponent id="stop" class="fixed" min-width="68" max-width="68"/>
            <ReferenceLabComponent id="seek" class="fill"/>
          </View>
          <View class="row">
            <ReferenceLabComponent id="startOffset" flex-grow="2"/>
            <ReferenceLabComponent id="loopEnabled" class="fixed" min-width="70" max-width="70"/>
            <ReferenceLabComponent id="loopStart" flex-grow="2"/>
            <ReferenceLabComponent id="loopEnd" flex-grow="2"/>
          </View>
          <View class="row">
            <ReferenceLabComponent id="transportSync" class="fixed" min-width="82" max-width="82"/>
            <ReferenceLabComponent id="syncMode" class="fixed" min-width="125" max-width="125"/>
            <ReferenceLabComponent id="syncOffset" class="fixed" min-width="145" max-width="145"/>
            <ReferenceLabComponent id="fullLoop" class="fill"/>
            <ReferenceLabComponent id="resetWaveformView" class="fill"/>
          </View>
          <ReferenceLabComponent id="hostTempoStatus" class="fixed" min-height="24" max-height="24"/>
        </View>
        <View id="analysisPage" class="page" tab-caption="Analysis">
          <View class="row">
            <ReferenceLabComponent id="fftSize" class="fixed" min-width="95" max-width="95"/>
            <ReferenceLabComponent id="fftSmoothing" class="fixed" min-width="160" max-width="160"/>
            <ReferenceLabComponent id="displayRate" class="fixed" min-width="90" max-width="90"/>
            <ReferenceLabComponent id="freezeDisplay" class="fixed" min-width="70" max-width="70"/>
            <ReferenceLabComponent id="resetMeters" class="fill"/>
          </View>
          <ReferenceLabComponent id="analysisDisplay" class="fill" min-height="180"/>
          <ReferenceLabComponent id="analysisMeters" class="fixed" min-height="118" max-height="118"/>
        </View>
        <View id="comparePage" class="page" tab-caption="Compare">
          <ReferenceLabComponent id="eqCurve" class="fill" min-height="150"/>
          <View class="row">
            <ReferenceLabComponent id="matchBypass" class="fixed" min-width="125" max-width="125"/>
            <ReferenceLabComponent id="autoMatch" class="fixed" min-width="110" max-width="110"/>
            <ReferenceLabComponent id="matchMode" class="fixed" min-width="150" max-width="150"/>
            <View class="fill"/>
          </View>
          <View class="row">
            <ReferenceLabComponent id="manualGain" flex-grow="2"/>
            <ReferenceLabComponent id="matchedGain" flex-grow="1"/>
          </View>
          <View class="row">
            <ReferenceLabComponent id="eqBypass" class="fixed" min-width="115" max-width="115"/>
            <ReferenceLabComponent id="listeningMode" class="fixed" min-width="135" max-width="135"/>
            <ReferenceLabComponent id="fade" class="fill"/>
          </View>
          <View class="row">
            <ReferenceLabComponent id="hpfEnabled" class="fixed" min-width="70" max-width="70"/>
            <ReferenceLabComponent id="hpf" flex-grow="2"/>
            <ReferenceLabComponent id="hpfSlope" flex-grow="1"/>
          </View>
          <View class="row">
            <ReferenceLabComponent id="bellEnabled" class="fixed" min-width="92" max-width="92"/>
            <ReferenceLabComponent id="bellFreq" class="fill"/>
            <ReferenceLabComponent id="bellGain" class="fill"/>
            <ReferenceLabComponent id="bellQ" class="fill"/>
          </View>
          <View class="row">
            <ReferenceLabComponent id="lpfEnabled" class="fixed" min-width="70" max-width="70"/>
            <ReferenceLabComponent id="lpf" flex-grow="2"/>
            <ReferenceLabComponent id="lpfSlope" flex-grow="1"/>
          </View>
        </View>
        <View id="detailsPage" class="page" tab-caption="Details">
          <View class="row">
            <ReferenceLabComponent id="filterArtist" class="fill"/>
            <ReferenceLabComponent id="filterGenre" class="fill"/>
            <ReferenceLabComponent id="filterKey" class="fill"/>
            <ReferenceLabComponent id="ratingFilter" class="fill"/>
          </View>
          <View class="row">
            <ReferenceLabComponent id="minBpm" class="fixed" min-width="90" max-width="90"/>
            <ReferenceLabComponent id="maxBpm" class="fixed" min-width="90" max-width="90"/>
            <ReferenceLabComponent id="cacheSize" class="fixed" min-width="135" max-width="135"/>
            <ReferenceLabComponent id="cacheStatus" class="fill"/>
            <ReferenceLabComponent id="clearCache" class="fixed" min-width="105" max-width="105"/>
          </View>
          <View class="row">
            <ReferenceLabComponent id="mixColourSelector" class="fill"/>
            <ReferenceLabComponent id="referenceColourSelector" class="fill"/>
          </View>
          <View class="row">
            <ReferenceLabComponent id="editTitle" class="fill"/>
            <ReferenceLabComponent id="editArtist" class="fill"/>
          </View>
          <View class="row">
            <ReferenceLabComponent id="editAlbum" class="fill"/>
            <ReferenceLabComponent id="editGenre" class="fill"/>
          </View>
          <View class="row">
            <ReferenceLabComponent id="editYear" class="fill"/>
            <ReferenceLabComponent id="editBpm" class="fill"/>
            <ReferenceLabComponent id="editKey" class="fill"/>
            <ReferenceLabComponent id="editRating" class="fill"/>
            <ReferenceLabComponent id="editFavourite" class="fill"/>
          </View>
          <View class="row">
            <ReferenceLabComponent id="editTags" flex-grow="1"/>
            <ReferenceLabComponent id="editNotes" flex-grow="1"/>
          </View>
          <View class="row">
            <ReferenceLabComponent id="save" class="fixed" min-width="145" max-width="145"/>
            <ReferenceLabComponent id="savePreset" class="fixed" min-width="115" max-width="115"/>
            <ReferenceLabComponent id="loadPreset" class="fixed" min-width="115" max-width="115"/>
            <ReferenceLabComponent id="safeExport" class="fill"/>
          </View>
          <View class="fill"/>
        </View>
      </View>
    </View>
  </View>
</magic>)xml";
    auto xml=juce::XmlDocument::parse(layout);
    if(xml==nullptr){jassertfalse;return{};}
    auto tree=juce::ValueTree::fromXml(*xml);
    setGuiMagicLibraryRatio(tree,libraryWidthRatio);
    return tree;
}
}
