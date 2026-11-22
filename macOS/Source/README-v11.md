# TrackerMac 0.11 — Creative Fill FX, audio quality and granular automation

## Creative Fill

Pattern (`1`) > REC (`R`) > Fill (`F4`). In Parameters, set **FX Amount** (0–100%). Default 50%; the last applied setting is saved in the project. Zero retains existing gate/retrigger generation but adds no new reverb, delay or slicer. Reverb (`v`), delay (`d`) and slicer (`s`) are injected in the two FX slots according to the musical role, without replacing gate or retrigger. Chords/lead get wider sends, percussion gets occasional slice/echo, and kicks/bass remain mostly dry. Random continues to generate different results. `D` in Pattern displays both FX slots.

## Granular Position Automation

Import a sample, select `9` and choose Granular. Press `A` (or F2 Position, then Enter). Alternatively, `3` Instrument Parameters > F7 Automation. This opens the per-instrument Granular Position automation editor.

- Up/Down selects Type, Shape, Speed or Depth. Q/E or Left/Right changes the value. Shift+Q/E accelerates numeric changes.
- Type: Off, LFO / Tempo sync, LFO / Free Hz.
- Shape: Triangle, Sine, Saw, Reverse Saw, Square, Random S&H.
- Sync Speed: 128, 96, 64, 48, 32, 24, 16, 12, 8, 6, 4, 3, 2, 3/2, 1 through fractional steps down to 1/64; these are cycle durations, not divisions of the audio sample. At 120 BPM, 128 steps = 16 seconds. The continuous phase crosses pattern loops without restarting; transport start resets it.
- Free Speed: 0.05–20.00 Hz, independent of BPM and pattern length. Depth: 0–100%; 100% scans across the selected Start/End sample region, while smaller depth centers the modulation around the Position setting.
- F1/Space previews, Option + pads or USB MIDI auditions. Enter/Esc closes. Position is modulated at each overlapping grain's creation (grain length, direction, shape and spread remain independent). The animated sample position marker and automation graph show the scan. Per-instrument settings are saved and rendered in exported songs.

Reference: Polyend Tracker manual, instrument automation and LFO speed, pp. 116–117; official instrument playback modes. https://polyend-website.fra1.digitaloceanspaces.com/wp-content/uploads/2023/11/29122938/Polyend-Tracker-Manual-1v9v2a.pdf

## Audio DSP changes

- Sample attack, region endpoints, note-off, gate, retrigger, loop boundaries and transport changes now have short anti-click ramps. Fader, mute, pan, effect sends and effect changes are smoothed. Sample preview duration follows actual pitch.
- Cubic interpolation for fractional sample positions and an 8-tap band-limited resampler for upward transposition. Saw/square oscillators use PolyBLEP; kick pitch and drum noise shaping are corrected.
- Reverb has 8 comb paths and 3 allpass diffusion stages per channel. Delay and reverb tap changes crossfade; feedback, damping and returns are smoothed.
- Master retains clean linear gain at normal levels, with 20% mix headroom and stereo-linked peak protection only near clipping. Existing samples remain mono internally, and the factory synths are deliberately lightweight.
- Config (`7`) > Audio DSP shows current sample rate, callback frames, render load, peak load and number of render blocks exceeding their timing budget. F5 resets statistics. This measures DSP execution time, not device round-trip latency or confirmed hardware xruns.

Validation: EngineQualityTest exercises discontinuities, upward-transposition alias suppression, finite/limited output and 192 granular voices. EngineLFOTest verifies audible region scanning, BPM-independent free rate, and 128-step sync across pattern loops. The existing workflow, MIDI, FX, sampler and export tests remain included.

Build as v0.10, additionally compiling `PositionAutomation.swift`.
