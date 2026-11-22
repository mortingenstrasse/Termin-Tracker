# TrackerMac 0.3 — hardware-inspired keyboard workflow

Native Apple Silicon prototype, not the official Polyend firmware. Based on the original Tracker manual (1.9.2) and official shortcut guide:

- https://polyend.com/manuals/tracker/
- https://backstage.polyend.com/t/what-are-the-polyend-trackers-shortcuts/245

## Physical controls

| Tracker control | Mac keyboard |
|---|---|
| Pattern | 1 |
| Song | 2 |
| Instrument Parameters | 3 |
| Master | 4 |
| Perform | 5 |
| File | 6 |
| Config | 7 |
| Sample Loader | 8 |
| Sample Playback | 9 |
| Sample Editor | 0 |
| Sample Recorder | - |
| Note / Instrument / FX1 / FX2 | N / I / F / G |
| Extra velocity / volume / pan columns | V / B / P |
| Play | Space |
| Rec | R |
| Copy / Paste | C / Shift+C |
| Insert / Home | T / Shift+T |
| Delete / Backspace | Delete / Shift+Delete |
| Shift | Shift |
| Navigation | Arrow keys |
| Jog wheel | Q (down), E (up) |
| 8 contextual screen buttons | F1–F8 or Cmd+1–8 |
| 48 pads | Option + four physical keyboard rows |

F1–F8 are the screen buttons, not the main menu keys. On Apple's media-key keyboards use Fn+F1–F8 or Cmd+1–8. To emulate holding a screen button while moving the jog wheel, hold F1/F2/F3 and press Q/E; releasing the function key commits the setting. Tapping a screen button opens its value picker, where Q/E or arrows and Enter work.

## 48 pads

Option held activates the pad layer, leaving the main keys available for hardware commands. The table uses US QWERTY physical positions; other keyboard layouts use the same positions. Four rows are 12 chromatic notes each, lowest octave at the bottom. [ and ] change the octave bank; for all 48 pads the highest base octave is 5.

| Octave relative to base | 12 physical pad keys |
|---|---|
| +3 | 1 2 3 4 5 6 7 8 9 0 - = |
| +2 | Q W E R T Y U I O P [ ] |
| +1 | A S D F G H J K L ; ' backslash |
| +0 | Z X C V B N M , . / Left Right |

Rec off: listen to notes without changing the pattern. Rec on: write the selected parameter; Step Jump advances after each pad entry. N selects Note, I selects the instrument, F/G select each FX slot. Tap a parameter then use Option+pad, or hold it and use Q/E or arrows. Enter on a note opens Notes/Chords; Tab switches tabs, arrows choose, Enter inserts, Escape cancels.

## Pattern workflow

1. Press 1; arrows navigate rows and tracks (not individual columns). The cursor is teal in normal mode.
2. Press R. The cursor turns red; editing is enabled.
3. Select N, I, F or G. Use pads to enter or Q/E to change a value; enter opens the selector.
4. F1/F2/F3 select Pattern, Length and Step Jump respectively. Shift+Up/Down or Q/E in a picker changes the value by ten.
5. Shift+arrows selects rectangular ranges; Shift+Up at top selects whole track, then again all tracks. C copies, Shift+C pastes. Hold N/I/F/G while pasting for selected-parameter paste. T inserts a blank step in selected tracks; Delete clears, Shift+Delete removes and shifts.
6. Hold 1 and press N/I/F/G to show eight tracks with one or two chosen fields. Press 1 alone to restore four tracks. Hold 1 and press arrows or Q/E to change pattern; 1+C copies entire pattern and 1+Shift+C pastes it.
7. Space plays/stops; Shift+Space starts at selected row. R+Space starts basic live recording; Shift+R+screen button arms a track. Shift+screen button mutes a track. In Master it solos instead.

Three footer pages cycle with F8: basic pattern controls, pattern operations, and naming. Pattern operations include basic repeated-step Fill, Preview, Undo/Redo, Invert, Duplicate, Expand, Shrink, Copy Pattern, Paste Pattern. Render is not yet implemented. Full advanced Fill algorithms are also pending.

## Song

2 opens the arrangement; R enables editing, T inserts a pattern row, Delete removes it, Q/E or Left/Right changes its pattern, Up/Down navigates. Space plays the arrangement at sample-counted step boundaries, looping after its final row. F4 taps tempo (four taps). Holding 2 + Q/E changes tempo; 4 + Q/E adjusts master volume. Song uses a basic serial pattern chain, not every original Song feature.

## Scope

The four sample-related page commands are mapped but display explicit placeholders. The instrument list still contains eight lightweight synth/noise voices. The full sample library, granular/wavetable, original FX collection, full Perform, sample recorder, rendering, MIDI, original firmware project format and measured latency are pending. FX1/FX2 independently support Gate, Retrig and Drive. Velocity, volume, pan, mute, solo, preview, and master gain affect audio. Existing 0.1/0.2 JSON sessions are supported.

## Build and validate

```sh
mkdir -p TrackerMac.app/Contents/MacOS
cp Info.plist TrackerMac.app/Contents/Info.plist
xcrun clang++ -std=c++20 -O3 -c EngineV2.cpp -o EngineV3.o
xcrun swiftc -parse-as-library -module-cache-path .build-cache -O -import-objc-header Engine.h TrackerV2.swift EngineV3.o -Xlinker -lc++ -o TrackerMac.app/Contents/MacOS/TrackerMac
codesign --force --sign - TrackerMac.app
TrackerMac.app/Contents/MacOS/TrackerMac --self-test
xcrun clang++ -std=c++20 -O3 EngineV2.cpp EngineV3Test.cpp -o engine-test
./engine-test
```
