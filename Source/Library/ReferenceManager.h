#pragma once
#include "LocalReferenceProvider.h"
#include "JsonCatalogProvider.h"
#include "HttpReferenceProvider.h"
#include "MetadataRepository.h"
#include <mutex>

namespace referencelab {
struct LibraryDescriptor { juce::String id,name; juce::File database; };
enum class ReferenceSort { title, artist, album, genre, year, bpm, rating, dateAdded, lastUsed };
struct ReferenceFilter {
    juce::String query;
    bool favouritesOnly = false;
    int minimumRating = 0;
    juce::String artist,genre,musicalKey;double minimumBpm=0,maximumBpm=999;
    ReferenceSort sort = ReferenceSort::title;
    bool ascending = true;
};
class ReferenceManager {
public:
    explicit ReferenceManager(juce::File database);
    void load(); bool save(juce::String& error);
    bool addFile(const juce::File&,juce::String& error);
    int scanFolder(const juce::File&,bool recursive,juce::String& warning);
    int importCatalog(const juce::File&,juce::String& warning);
    int importRemoteCatalog(const juce::String&json,const juce::URL&catalogUrl,juce::String&warning);
    bool addUrl(const juce::String&,juce::String& error);
    bool createPlaylist(const juce::String& name,juce::String& error);
    bool addToPlaylist(const juce::String& playlistId,const juce::String& referenceId,juce::String& error);
    bool removeFromPlaylist(const juce::String& playlistId,const juce::String& referenceId,juce::String& error);
    bool renamePlaylist(const juce::String&playlistId,const juce::String&name,juce::String&error);
    bool deletePlaylist(const juce::String&playlistId,juce::String&error);
    bool movePlaylistItem(const juce::String&playlistId,const juce::String&referenceId,int delta,juce::String&error);
    std::optional<ReferenceMetadata> selectPlaylistRelative(const juce::String& playlistId,int delta,juce::String& error);
    std::vector<ReferencePlaylist> playlistsSnapshot() const;
    std::vector<LibraryDescriptor> librariesSnapshot()const;
    juce::String activeLibraryId()const;
    bool createLibrary(const juce::String&,const juce::File&,juce::String&error);
    bool switchLibrary(const juce::String&,juce::String&error);
    bool renameLibrary(const juce::String&id,const juce::String&name,juce::String&error);
    bool unregisterLibrary(const juce::String&id,juce::String&error);
    bool relinkLibrary(const juce::String&id,const juce::File&folder,juce::String&error);
    bool remove(const juce::String& uuid,juce::String& error);
    bool updateMetadata(const ReferenceMetadata&,juce::String& error);
    bool relink(const juce::String&uuid,const juce::File&,juce::String&error);
    std::vector<ReferenceMetadata> search(const juce::String& query) const;
    std::vector<ReferenceMetadata> filter(const ReferenceFilter&) const;
    ReferenceLibrary snapshot() const;
    juce::String getLastWarning() const;
private:
    mutable std::mutex mutex; MetadataRepository repository; juce::File playlistsFile,registryFile; std::vector<LibraryDescriptor> libraries;juce::String activeLibrary;LocalReferenceProvider local; JsonCatalogProvider catalog; HttpReferenceProvider http; ReferenceLibrary library; std::vector<ReferencePlaylist> playlists; juce::String lastWarning;
    bool savePlaylists(const std::vector<ReferencePlaylist>&,juce::String& error) const;
    bool saveRegistry(const std::vector<LibraryDescriptor>&,const juce::String&,juce::String&error)const;
    static bool matches(const ReferenceMetadata&,const juce::String&);
};
}
