# TrackerMac 0.8 — Sample Engine, Granular + FX

Native Apple Silicon Tracker-inspired prototype; original Polyend firmware is not included. Existing 0.1–0.7 projects are compatible.

## Live step effects

In Pattern, enable REC, select `F` (FX1) or `G` (FX2), and press Enter. The effect popup lists all eight effects. Left/Right selects an effect; Up/Down or Q/E adjusts its value; Enter applies.

| Effect | Tracker code | Function |
|---|---|---|
| Off | --- | No effect |
| Gate | G | Step note gate |
| Retrig | R | Subdivides the remaining step duration |
| Drive | D | Soft saturation |
| Reverb | v | Stereo room send, 0–100% |
| Delay | d | BPM-synced dotted eighth ping-pong send, 0–100% |
| Slicer | s | Rhythmic amplitude chop, 2–8 subdivisions |
| Sample Slice | S | Select slice 1–48, independently of played note |

Reverb/Delay use shared stereo send buses with persistent audio tails, summed before the pattern and master faders. Slicer and Sample Slice are different: Slicer rhythmically gates a playing sound; Sample Slice selects an actual marker-delimited segment. FX1 and FX2 may be used together.

## Sample Editor (0)

Import audio into an instrument with `8` Sample Loader, then `0` Sample Editor. The editor displays the actual waveform, cached at screen resolution and recalculated on zoom. Use `I` and Q/E to choose another imported instrument.

| Screen button | Function |
|---|---|
| F1 | Play/stop selected region |
| F2 / Enter | Apply highlighted effect |
| F3 | Undo |
| F4 | Focus Start; Q/E moves the marker |
| F5 | Focus End; Q/E moves the marker |
| F6 | Focus Zoom; Q/E changes zoom |
| F7/F8 or Up/Down | Choose effect |

Mouse: click/drag on waveform to set the selected Start or End marker. Left/Right moves parameter focus; Q/E adjusts; Shift gives larger steps. Sample bounds are non-destructive until Crop is applied. The effects act only on the selected region.

- Crop: retains only the selected region and adjusts slice markers.
- Reverse: reverses sample frames in the selected region.
- Bitcrusher: `B` + Q/E sets 2–16 bit depth; `C` + Q/E sets 1–32x sample hold/downsample.
- Chop Even: `B` + Q/E sets 2–48 target slices, then Apply.
- Chop Transients: estimates transient locations from changes in short-window signal energy; Apply stores the resulting slice markers. If necessary, adjust them manually in Sample Playback.

All destructive PCM edits have Undo support. WAV/AIFF and other AVAudioFile-supported inputs are imported as mono float PCM (same sample memory limits as previous versions).

## Sample Playback (9)

Press `9` after selecting/importing an instrument. F7/F8 or Up/Down browses these real playback modes: 1-Shot, Forward Loop, Backward Loop, Pingpong Loop, Slice, Granular. F1 previews; Option+pad plays the chosen note or slice. The normal 1-Shot/Loop modes use the Start and End markers (F2/F3) as playback/loop boundaries; F4 selects zoom.

### Slice

In Slice mode the waveform displays numbered markers. C1 plays slice 1; C#1 plays slice 2, etc. Notes outside the slice range wrap. The original speed is preserved rather than pitch-shifting the slices. Pattern FX `S` also chooses a slice independently of the note. F2 focuses the cursor, F3 target slice count, F4 adds a marker at the cursor, F5 removes the nearest noninitial marker, F6 auto-chops. The mouse can place the cursor. The imported sample remains a single instrument slot; chop mode does not create duplicate PCM files.

### Granular

Choose Granular from the mode list. The waveform shows the grain position and region. Each note runs two overlapping, windowed grains with sample-rate-correct transposition and a per-grain start position. The voice continues until gated, replaced or stopped.

| Screen button | Control |
|---|---|
| F1 | Preview / Stop |
| F2 | Position (within selected sample region) |
| F3 | Grain length (1–1000 ms) |
| F4 | Shape: Square (de-clicked), Triangle, Gauss, Hann |
| F5 | Direction: Forward, Backward, Pingpong |
| F6 | Spread: random grain-position variation, 0–100% |

Select a control, then Q/E to adjust. Position and other grain parameters can be adjusted during playback. Click/drag the waveform to scan position. Polyphonic notes and chords use the same grain settings and individual pitches. The playback engine uses a compact, original C++ implementation, not a port of Mutable Instruments Clouds; Clouds is an available MIT-licensed reference if more granular features are needed later.

Sample bounds, slices, grain parameters, edited PCM and FX are saved in the project. No extra file dependencies are required. Live audio processing uses preallocated buffers and immutable sample/config snapshots; no PCM replacement/allocation in the render callback. Actual end-to-end audio latency is not benchmarked.

## Build and test

```sh
mkdir -p TrackerMac.app/Contents/MacOS
cp Info.plist TrackerMac.app/Contents/Info.plist
xcrun clang++ -std=c++20 -O3 -c EngineV2.cpp -o Engine.o
xcrun swiftc -parse-as-library -module-cache-path .build-cache -O -import-objc-header Engine.h TrackerV2.swift CreativeFill.swift CreativeFillUI.swift CreativeFillTests.swift Mixer.swift MixerTests.swift Sampler.swift SamplerTests.swift Engine.o -Xlinker -lc++ -o TrackerMac.app/Contents/MacOS/TrackerMac
codesign --force --sign - TrackerMac.app
TrackerMac.app/Contents/MacOS/TrackerMac --self-test
xcrun clang++ -std=c++20 -O3 EngineV2.cpp EngineV8Test.cpp -o engine-test
./engine-test
```

References: https://polyend.com/manuals/tracker/ ; https://github.com/pichenettes/eurorack/tree/master/clouds
