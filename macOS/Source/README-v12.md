# TrackerMac 0.12 — Anchor scan, live sampling, pattern copy

## Background

Main background and ordinary Pattern rows now use exact sRGB `#18191C`; every fourth row has a slightly brighter related shade. Home, automation and copy dialogs use the new background.

## Granular position anchors

On Sample Playback (`9`) select Granular, then `A` (or `F2` Position > Enter). The automation screen now has the selected sample region's waveform and five independent markers, numbered 0–4. Select a marker with the number key or arrow navigation, then click the waveform or adjust with Q/E (Shift for larger changes). Editing a marker switches Full region to Anchors / Step. Positions are relative to Start/End and can be in any order (e.g. 10%, 75%, 35%, 90%, 20%).

- Full region: original continuous LFO sweep.
- Anchors / Step: maps LFO phase to the five positions, so new grains begin at those defined anchors only. Position scatter is suppressed in this mode so it does not defeat the anchor selection.
- Anchors / Morph: interpolates between adjacent anchors in numbered order. Spread still applies.

Type, shape, speed and depth remain adjustable. Set Type to LFO / Tempo sync or LFO / Free Hz. LFO depth 100% travels across all five positions; smaller values traverse a subset around the middle anchor; zero uses manual Position. F1 previews, Enter/Esc closes. Both anchor settings and LFO are stored in the instrument and in exported audio. Existing projects retain Full region mode. Polyend's stock manual describes continuous granular position modulation; the five-anchor scan is a custom extension.

## Live Record

`8` Sample Loader > F4 Live Record (or `-`) opens the recorder. It uses macOS's current default audio **input**, such as the internal microphone or a connected USB audio interface. Choose the desired input in macOS System Settings > Sound > Input. On first record, macOS asks for microphone access. No recording starts before explicit Record.

F1 (or R/Space) starts/stops a take; waveform updates in real time. F2 previews the recorded audio. Q/E, arrows, or F6/F7 select a destination instrument slot; occupied slots require confirmation. F3 adds a normal sample. F4 adds a granular sample and opens Sample Playback with the recorded instrument selected. F5 discards the take. F8 returns to Sample Loader. Leaving recorder stops the capture and retains the take in memory until discarded or replaced. The resulting instrument is the same as a disk-imported one and is embedded when the project is saved.

Recording stores float32 mono PCM (first two input channels downmixed) at the input device's sample rate, up to 1,000,000 frames per take (20.8 s at 48 kHz). No direct system-output capture or live input monitoring is enabled; a loopback input device can be selected externally if desired. The recorder buffer is separate from playback DSP and preallocated.

## Copy Pattern To

On Pattern, F1 opens the Pattern selector (or select Pattern footer and Enter). Press F4/Tab or click Copy this pattern to. Use Q/E or arrows (Shift for ×10), select destination P00–P127, then Enter. An occupied destination asks for confirmation. The entire pattern, including notes, FX, microtiming, track assignments, expanded track count, and pattern mixer, is copied, and the destination becomes selected. Source remains intact; undo restores the previous project. The song chain is not changed.

## Validation

EngineCaptureScanTest verifies captured PCM downmix, live waveform peaks, exact anchor selection and morphing. Sampler self-test verifies anchor settings persistence, captured sample to granular slot, and independent pattern copy. Physical input capture needs testing on a Mac with microphone permission and the desired input device.

Build as v0.11, additionally compiling CaptureDSP.cpp, LiveRecorder.swift and PatternCopy.swift.
