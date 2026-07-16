# ReferenceLab 1.0

Plugin VST3/Standalone C++20 basato su JUCE per il confronto A/B con reference locali.

## Build Windows

```powershell
cmake -S . -B build -G "Visual Studio 17 2022" -A x64
cmake --build build --config Release --target ReferenceLab_VST3
```

La prima configurazione scarica JUCE 8.0.8. Il VST3 viene generato sotto `build/ReferenceLab_artefacts/Release/VST3`.

La modalità Safe Export è attiva per impostazione predefinita: durante `isNonRealtime()` passa soltanto il mix.

La v1 include libreria JSON locale, scansione ricorsiva, ricerca e metadati modificabili, player con loop/seek/resampling, cache LRU asincrona configurabile, confronto A/B, EQ simmetrico e modalità Stereo/Mono/Mid/Side.

## Scorciatoie

- `B`: alterna Mix e Reference.
- `Spazio`: Play/Pause della reference quando l'editor ha il focus.

## Rendering offline

Safe Export è attivo per impostazione predefinita. Alcuni host potrebbero non comunicare correttamente la modalità non real-time: verificare sempre un export di prova nella DAW utilizzata.

## Stato della versione 1

Il core VST3 è implementato. Prima della distribuzione pubblica sono obbligatori test manuali in almeno tre DAW Windows, misure CPU/latency e validazione delle metriche con segnali di laboratorio.
