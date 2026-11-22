# TrackerMac 0.4 — Apple Silicon

Independent native Tracker-inspired prototype. Fullscreen, keyboard-first, 24 polyphonic tracks, two pages of 12 compact tracks, 48 instrument slots, and sample-accurate positive microtiming. The original Polyend firmware is not included.

## Start and save

At startup, choose New, Load or Demo with arrows and Enter. New projects have 24 unassigned tracks; the factory instrument pool includes eight built-in voices and 40 empty slots. H returns to the home menu (Resume restores the current session). 6 opens File: F1 New, F2 Open, F5 Save, F6 Save As, F8 Home. Cmd+S and Cmd+O also work; Cmd+Shift+S saves a copy. Existing v0.1–0.3 project formats migrate to 24 tracks. Imported sample PCM is embedded in the project.

## Pattern layout and controls

- 1: Pattern; 12 tracks per page. Page Up/Down (Fn+Up/Down on a compact Mac keyboard), Ctrl+Left/Right, or moving past the edge with the arrow keys changes track page.
- D: full detail mode (four tracks at a time). Compact mode displays Note / Instrument / Time, or the selected parameter in the third column. The underlying velocity, volume, pan, two FX and timing values remain independent.
- R enters REC mode. N Note, I Instrument, V Velocity, B Volume, P Pan, F FX1, G FX2, M Micro-time. Q/E adjusts the selected parameter; Enter opens its selector. Arrow keys move between rows/tracks.
- Notes/Chords picker: Tab changes tab, Q/E changes octave, arrows select, Enter commits.
- M then Enter: 0.00, +0.25, +0.50, +0.75 of a step. Q/E adjusts in quarter-step increments. At 120 BPM, one step is a sixteenth note (125 ms), so +0.25 delays the event by 31.25 ms. The offset is scheduled inside the audio engine before the note trigger; it is not a graphical change only. Use the preceding row for a note that needs to fall just before the next grid line.
- Space play/stop; Shift+Space from marker; R+Space live record. Shift+M mutes selected track; Shift+S solos it.
- C copies, Shift+C pastes, T inserts, Shift+T jumps to top; Shift+arrows selects multiple steps. F1–F8 remain contextual screen keys, with Cmd+1–8 as alternatives.

## Instruments

1. From a blank track, select I then Enter to open the 48-slot instrument list. Use Q/E or Up/Down; choose a populated slot with Enter. This sets the track default for future note entry. With REC on, changing I on a populated step changes that step's instrument.
2. 3 opens Instrument Parameters: select instrument with Up/Down, target track with Left/Right, Enter or F1 assigns the instrument to all notes of that track across existing patterns and to future notes. F2/F3 change the selected slot's built-in voice; F5/F6 adjust a sample's root note. Option+pad auditions.
3. 8 opens the Sample Loader. Left focuses file pane; Enter opens the native file browser (choose WAV/AIFF or another supported audio file). Right focuses the instrument pool; Up/Down chooses a slot. F1 Add imports to that slot; F2 Add Next also selects the next slot. Replacing a populated slot asks first. Return to Pattern and select the slot using I then Enter. Mono one-shot playback is pitched from its configurable root (default C4). The import downmixes multi-channel files to mono. Limit: 1,000,000 frames per file and 64 MB total decoded sample data.

## BPM

As on the hardware, hold Song (2) and use Q/E or Up/Down for 1 BPM changes; add Shift for 0.1 BPM. Tapping 2 alone opens Song. In Song, hold F8 Tempo and turn Q/E or Up/Down; tap it four times for tap tempo. Click the displayed BPM or press = to open direct numeric entry. Range 40–800 BPM.

## Build/test

```sh
mkdir -p TrackerMac.app/Contents/MacOS
cp Info.plist TrackerMac.app/Contents/Info.plist
xcrun clang++ -std=c++20 -O3 -c EngineV2.cpp -o Engine.o
xcrun swiftc -parse-as-library -module-cache-path .build-cache -O -import-objc-header Engine.h TrackerV2.swift Engine.o -Xlinker -lc++ -o TrackerMac.app/Contents/MacOS/TrackerMac
codesign --force --sign - TrackerMac.app
TrackerMac.app/Contents/MacOS/TrackerMac --self-test
xcrun clang++ -std=c++20 -O3 EngineV2.cpp EngineV4Test.cpp -o engine-test
./engine-test
```

Remaining limitations: sampler is mono one-shot with pitch, not the complete Polyend granular/wavetable/sample-editing system. No external AU/MIDI or measured end-to-end latency yet. Original Tracker project/instrument formats not supported. Sample Editor/Playback/Recorder remain incomplete. Song arrangement is a simple pattern chain. 12-track grid uses compact columns so that it stays readable.
