# ReferenceLab 1.0

Plugin VST3/Standalone C++20 basato su JUCE per il confronto A/B con reference locali.

## Build Windows

```powershell
cmake -S . -B build -G "Visual Studio 17 2022" -A x64
cmake --build build --config Release --target ReferenceLab_VST3
```

La prima configurazione scarica JUCE 8.0.8. Il VST3 viene generato sotto `build/ReferenceLab_artefacts/Release/VST3`.

La modalità Safe Export è attiva per impostazione predefinita: durante `isNonRealtime()` passa soltanto il mix.
