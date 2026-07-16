# ReferenceLab – Requisiti (v1)

## Tecnologie
- C++20
- JUCE
- VST3, Windows

## Obiettivo
Plugin professionale per confronto A/B tra il mix della DAW e reference audio, con strumenti di analisi integrati.

## Funzionalità principali

### A/B
- Switch istantaneo Mix/Reference
- Crossfade configurabile (0–100 ms)
- Nessun click/pop
- Playback indipendente della reference

### Reference Library
Supporto iniziale:
- File locali (WAV, AIFF, FLAC, MP3)

I file vengono catalogati tramite `reference.json`.

Esempio:

```json
[
  {
    "title": "",
    "artist": "",
    "album": "",
    "genre": "",
    "year": ,
    "bpm": ,
    "key": "",
    "rating": ,
    "favorite": ,
    "notes": "",
    "src": ""
  }
]
```

Funzioni:
- ricerca
- filtri
- ordinamento
- modifica metadati dal plugin

## Loudness Matching
- Automatico
- Integrated / Short / Momentary LUFS
- Gain manuale opzionale

## Loop
- Marker A/B
- Loop continuo
- Trascinamento marker

## Analisi

Disponibili per Mix, Reference e Overlay:

- FFT Spectrum
- Oscilloscopio
- Correlation Meter
- Stereo Width
- LUFS
- Peak
- RMS
- Crest Factor
- Dynamic Range

## Modalità di confronto

Le elaborazioni devono essere applicate contemporaneamente ad entrambe le sorgenti.

### EQ Parametrico (solo monitoraggio)

- High Pass
- Bell (Band Pass)
- Low Pass

Controlli:
- Frequenza
- Gain (Bell)
- Q (Bell)

Scopo:
- confronto delle basse
- confronto delle alte
- confronto di bande specifiche

### Modalità di ascolto

- Stereo
- Mono
- Mid
- Side

Applicate contemporaneamente a Mix e Reference.

## Prestazioni

- Latenza trascurabile
- CPU contenuta
- Caricamento asincrono
- Cache audio

## Rendering Offline

Il plugin deve rilevare il rendering offline (offline bounce/export) tramite le API JUCE (`AudioProcessor::isNonRealtime()`).

Durante il rendering:

- disabilitare automaticamente il playback della reference;
- non processare la reference;
- bypassare le elaborazioni di confronto;
- non influenzare il file esportato.

Alla fine del rendering il plugin deve ripristinare automaticamente lo stato precedente.

## Roadmap

### v1
- File locali
- Libreria JSON
- Analizzatori
- EQ
- Mid/Side
- Mono
- Loudness Matching
- Loop

### v2
- URL HTTP
- Cambridge MT
- Import cataloghi JSON
-Air Windows meter