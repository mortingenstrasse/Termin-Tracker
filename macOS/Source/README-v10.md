# TrackerMac 0.10 — Song export and FX settings

## Export audio

Open Song (`2`) and press `F4` (or `Cmd+4`): Export Audio. Choose a destination. The complete Song chain is rendered once, including repeated patterns, all track/pattern mixer settings, master gain, samples, granular voices and step FX. The result is stereo 48 kHz / 24-bit PCM WAV. An automatically calculated effects tail follows the end (up to 31 seconds, with a short fade at the very end). Esc cancels while export is running. Pattern (`1`) > More (`F8`) > Render (`F1`) renders just that pattern.

Export snapshots the project and renders offline in a separate process, using the same engine; the actual live audio session is not changed. The export writes a temporary WAV and installs it at the chosen destination only after success. MIDI audition and temporary preview sounds are excluded.

## FX settings

Press `X` from any main editing page (no popup open). Reverb and Delay are shared project-level send effects; the `v` and `d` step effects still determine individual send amounts. Play a pattern/song with sends to audition settings live.

- Up/Down: parameter; Left/Right or Tab: Reverb / Delay; Q/E: decrease/increase (Shift for larger steps).
- F1/F2: previous/next reverb preset. F3/F4: select Reverb/Delay. F5: reset both. F6/F7: undo/redo. F8 or Esc: Mixer.
- Reverb presets: Room, Hall, Plate, Chamber, Ambient. These are parameter presets of the same original stereo reverberator, not hardware/IR emulations. Editing a reverb parameter selects Custom. Decay 0.1–12 s, pre-delay 0–250 ms, size, damping, return level and stereo width 0–150%. Hall starts at 4.5 s and 125% width; Ambient at 9 s and 145% width.
- Delay: 1/32 through 1 bar including dotted eighth, feedback up to 90%, damping, return level and Stereo / Ping-pong modes.
- Settings are saved with the project; older projects load defaults. There are no new plugin dependencies. DSP buffers remain preallocated; settings are published atomically and applied at audio-block boundaries.

## Validation

`--self-test` tests existing Tracker workflows; `--export-self-test` checks a rendered A-B-A song chain, panning, WAV format, and project setting roundtrip. `EngineFXTest.cpp` tests reverb tail, pre-delay, stereo width and ping-pong response.

Build as v0.9, additionally compiling `FXSettings.swift AudioExport.swift`.
