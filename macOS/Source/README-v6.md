# TrackerMac 0.6 — Master Mixer

The native Apple Silicon tracker now has a mixer inspired by Polyend's Track Mixer page. Press `4` to open it. Twelve track channel strips are visible at once, with the three pattern/master controls always on the right.

## Mixer controls

| Control | Action |
|---|---|
| Left/Right | Select a track, Pattern L/R, or Master fader |
| Q/E or Up/Down | Adjust the selected fader |
| Shift + Q/E or Up/Down | Adjust ten units |
| Fn+Up/Down (Page Up/Down) | Tracks 1–12 / 13–24 |
| Ctrl+Left/Right | Alternative track bank navigation |
| M / F1 | Mute selected track |
| S / F2 | Toggle solo on selected track (multiple solos allowed) |
| F3 or Delete | Reset selected fader to unity |
| F4/F5 | Previous/next track bank |
| F6/F7/F8 | Select Pattern L / Pattern R / Master Output |
| Tab | Cycle to Pattern L/R and Master, then track |
| Mouse | Drag faders; click individual M/S buttons |

Track faders are global across the project; Pattern L/R faders are independent and saved for each pattern. During song playback the Pattern bus follows the active pattern. Master controls the overall stereo output. Gain is 0–120%, with 100% = 0 dB and 120% = +6 dB; zero is silent. All mixer parameters, mute and solo states are stored in the project. Mutes are remembered while soloing; if one or more tracks are soloed only those tracks sound, and cancelling solo restores the previous mute configuration.

Meters measure real audio samples: each track after its fader/mute, Pattern L/R after the pattern faders but before master, and Master L/R after the final output stage. A red peak indicator warns if the signal exceeds nominal full scale; the output retains the previous soft saturation stage. Gain sliders ramp smoothly except zero, which silences immediately. Peak envelopes decay on stop. The audio callback publishes peak values without allocating memory; the UI reads them asynchronously.

Existing 0.1–0.5.1 projects load with unity mixer levels and no mutes/solos when these settings are absent. All prior Creative Fill, playback follow and sequencing controls remain. See `README-v5.md` for previous features; use the updated build command below.

## Build and tests

```sh
mkdir -p TrackerMac.app/Contents/MacOS
cp Info.plist TrackerMac.app/Contents/Info.plist
xcrun clang++ -std=c++20 -O3 -c EngineV2.cpp -o Engine.o
xcrun swiftc -parse-as-library -module-cache-path .build-cache -O -import-objc-header Engine.h TrackerV2.swift CreativeFill.swift CreativeFillUI.swift CreativeFillTests.swift Mixer.swift MixerTests.swift Engine.o -Xlinker -lc++ -o TrackerMac.app/Contents/MacOS/TrackerMac
codesign --force --sign - TrackerMac.app
TrackerMac.app/Contents/MacOS/TrackerMac --self-test
xcrun clang++ -std=c++20 -O3 EngineV2.cpp EngineV6Test.cpp -o engine-test
./engine-test
```

Reference: https://polyend.com/manuals/tracker/ — Master Pages / Track Mixer.
