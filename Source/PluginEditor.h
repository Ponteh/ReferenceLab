#pragma once
#include <juce_gui_extra/juce_gui_extra.h>
#include "PluginProcessor.h"
#include "UI/AnalysisDisplay.h"
#include "UI/ReferenceWaveform.h"
#include "UI/EqCurveDisplay.h"
#include "UI/PanelResizer.h"
#include "UI/AnalysisMeters.h"
#include "UI/GuiMagicTheme.h"
#include "UI/GuiMagicComponentItem.h"
#include "UI/GuiMagicLookAndFeel.h"
#include "UI/AirwindowsMeter.h"
#include <map>

class ReferenceLabEditor final:public juce::AudioProcessorEditor,private juce::ListBoxModel,private juce::Timer{
public: explicit ReferenceLabEditor(ReferenceLabAudioProcessor&);~ReferenceLabEditor()override;void paint(juce::Graphics&)override;void resized()override;bool keyPressed(const juce::KeyPress&)override;
private:
    int getNumRows() override{return (int)rows.size();}
    void paintListBoxItem(int,juce::Graphics&,int,int,bool) override;
    void listBoxItemDoubleClicked(int,const juce::MouseEvent&) override;
    void selectedRowsChanged(int) override;
    void timerCallback() override;
    void refreshLibrary(); void refreshPlaylists(); void refreshLibraries(); void chooseFolder(); void chooseCatalog(); void showError(const juce::String&); void saveSelected(); void removeSelected();void configurePlayback(int);void playItem(const referencelab::ReferenceMetadata&);void applyLoop();void persistPlayback();void applyTheme();void updateSourceStatus();void updateCacheStatus();
    ReferenceLabAudioProcessor&p;juce::Component magicSurface;std::unique_ptr<foleys::MagicGUIBuilder>magicBuilder;referencelab::AnalysisDisplay analysisDisplay;referencelab::AnalysisMeters analysisMeters;referencelab::ReferenceWaveform waveform;referencelab::EqCurveDisplay eqCurve;referencelab::AirwindowsMeter airwindowsMeter;referencelab::PanelResizer mainSplitter;juce::TextButton libraryDrawer{"LIBRARY"},fileMenu{"FILE"},catalogMenu{"CATALOG"},settingsButton,add{"Add File"},scan{"Scan Folder"},importCatalog{"Import JSON"},addUrl{"Web Source"},remove{"Remove"},relink{"Relink"},save{"Save Metadata"},savePreset{"Save Preset"},loadPreset{"Load Preset"},resetMeters{"Reset Meters"},newLibrary{"New Library"},newPlaylist{"New"},addToPlaylist{"+ Playlist"},previousReference{"<"},nextReference{">"},mix{"MIX"},ref{"REFERENCE"},play{"Play"},pause{"Pause"},stop{"Stop"},resetWaveformView{"Full View"},fullLoop{"Full Loop"},clearCache{"Clear Cache"};
    std::map<juce::String,juce::Component*>magicComponents;
    juce::Label title,matchedGain,transportStatus,hostTempoStatus,cacheStatus;juce::TextEditor search,filterArtist,filterGenre,filterKey,minBpm,maxBpm,editTitle,editArtist,editAlbum,editGenre,editYear,editBpm,editKey,editRating,editTags,editNotes;juce::TextButton recursiveScan{"Subfolders"},favouritesOnly{"Favorites Only"},sortDescending{"Desc."},editFavourite{"Favorite"},autoMatch{"Auto Match"},matchBypass{"Match Bypass"},freezeDisplay{"Freeze"},swapLR{"Swap L/R"},showMidSpectrum{"MID"},showSideSpectrum{"SIDE"},safeExport{"Safe Export"},loopEnabled{"Loop"},eqBypass{"EQ Bypass"},hpfEnabled{"HPF"},bellEnabled{"Band Pass"},lpfEnabled{"LPF"},transportSync{"DAW Sync"};juce::ComboBox sort,ratingFilter,cacheSize,matchMode,syncMode,listeningMode,hpfSlope,lpfSlope,fftSize,displayRate,mixColourSelector,referenceColourSelector;juce::Slider manualGain,seek,startOffset,loopStart,loopEnd,fade,hpf,bellFreq,bellGain,bellQ,lpf,syncOffset,fftSmoothing,analysisSlope;juce::ListBox list{"Reference Library",this};
    std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment>autoMatchAttachment;std::unique_ptr<juce::AudioProcessorValueTreeState::ComboBoxAttachment>matchModeAttachment,syncModeAttachment;std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment>manualGainAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment>eqBypassAttachment;std::unique_ptr<juce::AudioProcessorValueTreeState::ComboBoxAttachment>listeningModeAttachment;std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment>fadeAttachment,hpfAttachment,bellFreqAttachment,bellGainAttachment,bellQAttachment,lpfAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment>transportSyncAttachment;std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment>syncOffsetAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment>matchBypassAttachment,swapLRAttachment,showMidSpectrumAttachment,showSideSpectrumAttachment,safeExportAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment>hpfEnabledAttachment,bellEnabledAttachment,lpfEnabledAttachment;std::unique_ptr<juce::AudioProcessorValueTreeState::ComboBoxAttachment>hpfSlopeAttachment,lpfSlopeAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ComboBoxAttachment>fftSizeAttachment,displayRateAttachment;std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment>fftSmoothingAttachment,analysisSlopeAttachment;
    std::vector<referencelab::ReferenceMetadata> rows;std::vector<referencelab::ReferencePlaylist> playlistRows;std::vector<referencelab::LibraryDescriptor>libraryRows;juce::ComboBox playlist,librarySelector;juce::Colour appliedMixColour,appliedReferenceColour;double libraryWidthRatio=.34,splitterDragStartRatio=.34;bool libraryOpen=true,settingsOpen=false;
};
