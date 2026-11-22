#!/bin/bash
set -euo pipefail
cd "$(dirname "$0")"
if [[ $(uname -s) != Darwin ]]; then
  echo 'This application must be built on macOS.'; exit 1
fi
if ! xcrun --find swiftc >/dev/null 2>&1; then
  echo 'Install Xcode Command Line Tools: xcode-select --install'; exit 1
fi
src="$PWD/Source"
output="$PWD/TerminTracker-0.19.1.app"
if [[ -e "$output" ]]; then
  echo 'TerminTracker-0.19.1.app already exists; move it before rebuilding.'; exit 1
fi
stage=$(mktemp -d "$PWD/.tracker-build.XXXXXX")
bundle="$stage/TerminTracker-0.19.1.app"
mkdir -p "$bundle/Contents/MacOS" "$bundle/Contents/Resources" "$stage/cache"
cd "$src"
xcrun clang++ -std=c++20 -O3 -c EngineV2.cpp -o "$stage/Engine.o"
xcrun clang++ -std=c++20 -O3 -c CaptureDSP.cpp -o "$stage/Capture.o"
xcrun swiftc -parse-as-library -module-cache-path "$stage/cache" -O -import-objc-header Engine.h \
  TrackerV2.swift CreativeFill.swift CreativeFillUI.swift CreativeFillTests.swift \
  Mixer.swift MixerTests.swift Sampler.swift SamplerTests.swift MIDI.swift FXSettings.swift \
  AudioExport.swift PositionAutomation.swift LiveRecorder.swift PatternCopy.swift Duration.swift MixFX.swift TimeStretch.swift Dynamics.swift Omondeu.swift AudioSetup.swift ComputerKeyboard.swift TerminTheme.swift \
  "$stage/Engine.o" "$stage/Capture.o" -Xlinker -lc++ -o "$bundle/Contents/MacOS/TerminTracker"
cp Info.plist "$bundle/Contents/Info.plist"
cp Termin.icns "$bundle/Contents/Resources/Termin.icns"
codesign --force --sign - "$bundle"
codesign --verify --deep --strict "$bundle"
"$bundle/Contents/MacOS/TerminTracker" --self-test
"$bundle/Contents/MacOS/TerminTracker" --export-self-test
mv "$bundle" "$output"
echo "Termin Tracker built: $output"
