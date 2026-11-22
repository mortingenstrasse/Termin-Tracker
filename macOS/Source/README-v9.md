# TrackerMac 0.9 — USB MIDI and polyphonic recording

TrackerMac now uses macOS CoreMIDI (MIDI 1.0 Universal MIDI Packets) to automatically connect to all available MIDI sources, including class-compliant USB keyboards. Devices can be connected or disconnected while the app is running; no special driver is needed for class-compliant hardware. Other keyboards may need the manufacturer's driver. No MIDI output, external clock sync, MPE or pitch bend is implemented yet.

## Play

- Connect the keyboard to the Mac and open Config (7). The MIDI source names appear under USB MIDI. If the device is not listed, check macOS Audio MIDI Setup / MIDI Studio.
- Pattern (1): select a track with an assigned instrument. REC off plays it without changing the project. Instrument (3), Sample Loader (8), Sample Playback (9) and Sample Editor (0) audition their selected instrument instead.
- For granular, import an audio sample with 8, assign it to a track with 3/F1, open 9, choose Granular with F7/F8 and play the USB keyboard. Up to eight MIDI voices sound independently, including note release and CC64 sustain. Note-on velocity changes amplitude; the selected sample root controls pitch. MIDI audition passes directly to Master like the existing preview.
- Config (7): F1/F2 decrease/increase the chord collection window (25–150 ms, default 65 ms); F3 releases MIDI notes (panic); F4 toggles live recording quantization between sixteenth grid and quarter-step microtiming. These are session-level input preferences.

## Record

- Step recording: Pattern (1), choose target track, assign an instrument, press R to enable REC (red), and play a note or chord. Notes arriving within the chord window are collected in one cell; a fully released staccato group closes early. The cursor advances by Step Jump only once per group. The cell contains up to eight notes, an inferred chord label when possible, and average velocity. The notes sound immediately while grouping happens separately.
- Real-time recording: stop playback first, turn REC on, hold R and press Space to start live recording. All notes go to the selected track (if tracks are explicitly armed, select an armed one). The first note captures the playhead row and phase; nearby chord notes are merged even when a grid boundary falls between them. The chord window is capped at 75% of a sixteenth in live mode to avoid swallowing fast phrases. In quarter-step mode the onset rounds to the nearest 0, +0.25, +0.50, +0.75 or next grid step; Grid mode rounds to the nearest full step. Subsequent loop playback uses the stored event. Space stops recording.
- In live recording the final release of a chord attempts to write a Gate effect for a short note or an OFF event on an empty later row. Existing events/FX are not overwritten. The tracker stores a shared duration for the chord, not independent note lengths. While recording, the MIDI audition is separate from sequence playback.
- L in Pattern inserts a manual OFF event at the selected cell (REC on, assigned track); this is particularly useful for stopping granular and looping voices. Delete clears a cell. OFF is stored in the project and may be copied/pasted. A note-on replacing an OFF clears the stop flag.
- A Pattern step holds at most eight simultaneous notes. Arbitrary voicings remain intact even if they do not match a named chord. The displayed chord root is inferred from the pitch classes; the audio uses the exact played MIDI notes.

## Build/test

The native engine does not allocate or lock in the audio callback for MIDI voice processing. Monitoring responds on the next audio callback after delivery to the main input handler. End-to-end USB/audio latency depends on the system and has not been measured. Physical keyboard tests require a connected device.

```sh
xcrun clang++ -std=c++20 -O3 -c EngineV2.cpp -o Engine.o
xcrun swiftc -parse-as-library -module-cache-path .build-cache -O -import-objc-header Engine.h TrackerV2.swift CreativeFill.swift CreativeFillUI.swift CreativeFillTests.swift Mixer.swift MixerTests.swift Sampler.swift SamplerTests.swift MIDI.swift Engine.o -Xlinker -lc++ -o TrackerMac.app/Contents/MacOS/TrackerMac
codesign --force --sign - TrackerMac.app
TrackerMac.app/Contents/MacOS/TrackerMac --self-test
xcrun clang++ -std=c++20 -O3 EngineV2.cpp EngineMIDItest.cpp -o engine-midi-test
./engine-midi-test
```

Apple CoreMIDI reference: https://developer.apple.com/documentation/coremidi/midiinputportcreatewithprotocol(_:_:_:_:_:)
