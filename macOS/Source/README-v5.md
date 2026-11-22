# TrackerMac 0.5.1 — Creative Fill + Playback Follow

Native Apple Silicon prototype with 24 tracks, 48 instrument slots, 8-note polyphony, 128-step patterns, and the new non-destructive Creative Fill overlay. The original Polyend firmware is not included.

Playback follow: the Pattern viewport automatically scrolls as the green playhead passes the last visible row, returns to the top when a pattern loops, and follows the edit cursor again when playback stops.

## Creative Fill

1. Open Pattern (`1`), select a track, turn on REC (`R`). A Shift+arrow selection limits the fill to that region (including multiple selected tracks); without a selection, fills cover the selected track's entire pattern.
2. Press `F4` (or `Cmd+4`, or click Fill). The overlay has Category, Preset and Parameters columns. Left/right or Tab changes columns, up/down chooses items, Q/E changes parameter values. Shift+Q/E uses larger increments for percentages.
3. `F6` or `P` creates a preview using the selected preset. `F7` or `R` creates a fresh category-specific random preview. Repeat as often as desired. Space starts/stops the current pattern, including the preview.
4. Enter or `F8` commits the current preview (or generates the selected preset if not yet previewed). Esc restores the original range without saving any edits. Undo reverses an applied fill in one action.

| Category | Presets |
|---|---|
| Kick | Four on the floor, every 8 steps, Boom Bap, 2-Step Garage, Broken Beat, Euclidean 5/16, Trap Kick, Random |
| Snare | Backbeat 2 & 4, half-time, ghost notes, breakbeat, Jungle, syncopated clap, snare roll, Random |
| Hi-hat | 8ths, 16ths, offbeat, swung hats, trap 1/32 rolls, triplet rolls, 1/64 rolls, broken hats, Random |
| Bass | Root pulse, syncopated, walking, octave groove, 808 sparse, arpeggiated, ostinato, Random |
| Lead | Motif, question/answer, arpeggio, ascending/descending, sparse, syncopated, pentatonic-style phrase, Random |
| Chords | Triads, cinematic 7ths, jazz 9ths, offbeat stabs, gospel-style voicings, broken chords, long pads, Random |

Each active step includes notes, instrument, velocity, volume, pan and microtiming. Musical presets may include gate FX; drum rolls include the Retrig effect. Hat velocities are guaranteed distinct per 16-step measure; the audio engine subdivides the remaining step duration after the micro-delay and varies gain across retrigger hits. Standard interval rhythms remain fixed, while density shapes random/creative rhythm choices.

Melodic generation supports 18 scales, selectable root and octave, and five harmonic plans. Existing polyphonic chord cells or previously generated harmonic root hints influence subsequent melodic fills; random fills choose a fresh diatonic plan when using Common harmony and no existing harmonic context. Each random action samples a fresh seed, and identical consecutive candidate regions are retried. Chord extensions and inversion candidates are generated from the selected scale and scored for voice leading. The user can also select Swing, Humanize, Density, Replace or Empty-only mode. The selected instrument is preserved in Auto mode; on an unassigned track, the generator suggests a factory voice (including a newly added synthesized snare). Choose a specific slot in Parameters to override it. Imported drum samples default to their sample root note.

## Other controls retained from 0.4

- `1` Pattern, `2` Song, `3` Instrument, `4` Master, `6` File, `8` Sample Loader.
- `D` toggles 12-track compact view / 4-track detailed view; Fn+Up/Down switches 12-track pages. `R` REC, `N` Note, `I` instrument, `V` velocity, `B` volume, `P` pan, `F/G` FX1/2, `M` microtiming, `Q/E` jog. Enter edits selected field. Notes/Chords popup uses Tab to switch and Q/E for octave.
- Hold `2` + Q/E tempo; Shift for tenths. `=` numeric tempo picker. `Space` play/stop.
- `8` opens Sample Loader, Enter in left pane browses audio files, Right focuses slots, F1 Add. `3` assigns selected instrument to a track, `I` + Enter assigns to a step or blank track.
- `6` File: F1 New, F2 Open, F5 Save, F6 Save As. Cmd+S / Cmd+O. H Home. Startup New/Load/Demo.
- C Copy, Shift+C Paste, T Insert, Shift+T Home, Shift+arrows select range. F6/F7 on the normal Pattern footer remain Undo/Redo.

## Implementation notes

Creative Fill runs entirely on the UI thread before publishing step data to the existing native sample-timed audio engine; no generative processing in the audio callback. Samples are still mono one-shots; full granular/wavetable/advanced sample editing and external AU/MIDI are pending. The project keeps its own instrument/sample data and harmonic root/scale settings; it is not compatible with Polyend's original project format.

## Build and tests

```sh
mkdir -p TrackerMac.app/Contents/MacOS
cp Info.plist TrackerMac.app/Contents/Info.plist
xcrun clang++ -std=c++20 -O3 -c EngineV2.cpp -o Engine.o
xcrun swiftc -parse-as-library -module-cache-path .build-cache -O -import-objc-header Engine.h TrackerV2.swift CreativeFill.swift CreativeFillUI.swift CreativeFillTests.swift Engine.o -Xlinker -lc++ -o TrackerMac.app/Contents/MacOS/TrackerMac
codesign --force --sign - TrackerMac.app
TrackerMac.app/Contents/MacOS/TrackerMac --self-test
xcrun clang++ -std=c++20 -O3 EngineV2.cpp EngineV5Test.cpp -o engine-test
./engine-test
```

Polyend's original Fill workflow reference: https://polyend.com/manuals/tracker/
