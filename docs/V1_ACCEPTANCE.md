# ReferenceLab v1 - Acceptance status

## Implemented

- VST3/Standalone stereo, transparent Mix path and Safe Export.
- Local WAV, AIFF, FLAC and MP3 provider with asynchronous loading.
- Versioned, atomic JSON library with backup and Unicode metadata.
- Recursive/flat scanning, search, dedicated filters, sorting, missing-file indication and relinking.
- Multi-reference model and configurable LRU memory cache.
- Play, pause, stop, seek, start offset, A/B loop and loop crossfade.
- Click-safe Mix/Reference routing and listening-mode smoothing.
- Symmetric HPF/Bell/LPF EQ with individual bypass and 12/24 dB slopes.
- Stereo, Mono, Mid and Side monitoring.
- K-weighted gated loudness, Peak, 4x interpolated True Peak, RMS, Crest, LRA, Correlation and Width.
- Automatic/manual loudness matching with smoothing, safety limit and bypass.
- Configurable 1024/2048/4096 FFT overlay, smoothing, refresh rate and output oscilloscope.
- Meter reset/freeze, DAW transport sync/fallback, project-state restore and comparison presets.
- Keyboard A/B and Play/Pause shortcuts.

## Release validation still required

- REAPER, Ableton Live and FL Studio/Cubase VST3 integration passes.
- Offline export verification in each supported host.
- Audible click/pop test under rapid switching and short loops.
- CPU target measurement at 48 kHz / 512 samples and memory-limit stress test.
- 10,000-record library benchmark and missing-file stress test.
- LUFS/True-Peak comparison against calibrated laboratory vectors.

These items require host applications, audio hardware or external reference vectors and cannot be certified by unit tests alone.
