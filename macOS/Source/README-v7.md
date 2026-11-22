# TrackerMac 0.7 — Expandable Tracks and Pattern Mixer

## Expand tracks

New patterns start with 11 tracks and an ADD TRACK column at position 12. Navigate to that column in the Pattern page and press Enter (or double-click it). The pattern expands to 24 tracks, selects track 12, and the next Right press moves to track 13 on the second screen. Fn+Up/Down still switches pages. No note or instrument can accidentally be entered in the Add Track placeholder. Expansion is saved per pattern and may be undone before further work.

Existing project files remain compatible. Legacy patterns with content or assignments beyond track 11 are automatically expanded, so their data stays visible. New patterns have their own instrument defaults; assigning a track instrument on page 3 affects the current pattern instead of rewriting all song patterns.

## Pattern-specific mixer

Open Master/Mixer (`4`). A collapsed pattern has 11 channel strips; an expanded pattern has two groups of 12. Left/Right navigates continuously across track 12/13, automatically displaying the other bank. Fn+Up/Down or Ctrl+Left/Right remains an alternative. The three stereo Pattern and Master strips are always visible; F6/F7/F8 jump to Pattern L/R and Master.

- P opens the pattern selector (Up/Down and Enter). It lists created patterns and song references. Click the pattern header to open the same selector.
- F4/F5 select the previous/next pattern directly.
- A toggles automatic follow of the playing pattern. A manually selected pattern stays locked while Song plays; changing its mix does not change the currently playing arrangement. Track/pattern meters are blank while editing an inactive pattern; Master remains live.
- Mixer track labels reflect the selected pattern's actual instruments and number of note steps. Tracks with different instrument assignments or events are visibly distinct.
- Q/E or Up/Down adjusts the selected fader; Shift makes larger changes. M/F1 toggles mute, S/F2 toggles solo. F3/Delete resets a fader; mouse dragging and M/S buttons still work.

The per-track gain, mute and solo states, Pattern L/R gain, instrument assignment, and expanded track count are saved separately for each pattern. During Song playback the audio engine changes to the new pattern's mixer values at each pattern boundary. Master Output remains global. Old global mixer fields are retained as defaults when reading older sessions.

## Build/test

```sh
mkdir -p TrackerMac.app/Contents/MacOS
cp Info.plist TrackerMac.app/Contents/Info.plist
xcrun clang++ -std=c++20 -O3 -c EngineV2.cpp -o Engine.o
xcrun swiftc -parse-as-library -module-cache-path .build-cache -O -import-objc-header Engine.h TrackerV2.swift CreativeFill.swift CreativeFillUI.swift CreativeFillTests.swift Mixer.swift MixerTests.swift Engine.o -Xlinker -lc++ -o TrackerMac.app/Contents/MacOS/TrackerMac
codesign --force --sign - TrackerMac.app
TrackerMac.app/Contents/MacOS/TrackerMac --self-test
xcrun clang++ -std=c++20 -O3 EngineV2.cpp EngineV7Test.cpp -o engine-test
./engine-test
```
