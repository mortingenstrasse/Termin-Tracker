# Termin Tracker Pocket — hardware beta 0.3

This folder contains the native 8-track SDL2 port, source, Mac test binary,
demo project and WAV, and the Knulli build/update scripts. Version 0.2 has
run successfully on the RG35XX Plus; version 0.3 requires another hardware test.

## Verified on the development Mac

- Exactly 8 sequencer tracks in the DSP build, 128 patterns × 128 steps, 48 instruments.
- 640×480 pixel UI, 4 compact tracks / 2 detailed tracks; black/pastel Termin colors.
- Same C++ audio engine: Omondeu, sample/loop/granular playback, reverb/delay,
  slicer, per-track sends and EQ, default kick sidechain.
- Pattern step entry, chord templates, duration, copy/paste/undo, pattern copy,
  song chain, sample loader (WAV), sampler position/grain/LFO controls and mixer.
- 32 MB project PCM budget and 1 million frames per sample.
- Simplified Creative Fill (standard, syncopated, random) — not yet the entire
  desktop Fill library.
- Save/load of the supported project subset in desktop-shaped JSON, padded to
  24 tracks for the Mac's decoder. Projects with events on tracks 9–24 are
  rejected. Other desktop-only settings are not fully round-tripped yet.

The engine rendered a demo without non-finite samples, save/load passed, and
the existing Termin Tracker 0.19.1 Mac application rendered the generated
Pocket demo project successfully as stereo 24-bit WAV.

## Mac preview

Double-click Run-Mac-Preview.command. The binary uses the SDL2 library already
installed on the development Mac. Exit through R2 > Exit or Escape.

| Anbernic | Mac preview | Action |
|---|---|---|
| D-pad | arrows | Navigate, select fields |
| A | X | Edit / confirm / assign |
| B | Z | Back |
| X | S | Delete in REC; sample import in Instruments |
| Y | A | Preview |
| L1 / R1 | Q / R | Track or octave (note editor) |
| L2 | W (hold) | Modifier |
| R2 | E | Pattern actions |
| Start | Space | Play / stop |
| Select | Tab | Page chooser |
| Menu | Escape | Home menu / continue |
| L2 + A | W + X | Toggle REC |
| L2 + Up/Down | W + arrows | Edit cell value |
| L2 + Left/Right | W + arrows | Octave |
| L2 + X/Y/B | W + S/A/Z | Copy / paste / undo |

In Pattern, L2+A enters REC. Navigate to a note cell and A opens the
note/chord picker; select pitch and chord type using the arrows, preview
with Y, commit with A. R2 opens Fill, Duration, Note Off, Pattern Length,
Pattern Copy, Tempo and Detail View. Select opens the eight pages.
In Project, choose slot 1–8, A saves, Y loads. 

Version 0.2 starts with a two-second note animation, followed by NEW / LOAD /
DEMO. On the first handheld launch, follow the one-time physical controller
setup prompts. This maps the raw button, hat or trigger independently of the
Knulli SDL controller database. The last MENU binding can be skipped with
START if Knulli reserves that key. L2+SELECT also opens the home menu. R2 opens
and closes Pattern Actions; SELECT alone opens the page chooser. Recalibrate
by selecting CONTROLLER SETUP on the home menu. Saved projects are retained
across updates.

## Version 0.3 — sample and navigation usability

- Holding the D-pad repeats after 280 ms and accelerates during a long hold.
- INSTRUMENT shows the selected track and its current instrument. L1/R1 selects
  the track, A assigns the highlighted instrument to every existing note on
  that track, X opens the sample browser, Y auditions.
- The WAV browser retains folders (Kick, Snare, HiHat, Percussion, Bass, Synth),
  excludes macOS metadata, A opens a folder or loads a sample, Y auditions it,
  B returns to the parent folder, X cancels. Loading automatically assigns it
  to the selected track and opens the waveform page.
- SAMPLER opens the browser with A or X when empty; X can replace the sample.
  Arrows edit playback mode, start/end, root, octave, position and chop. Select
  CHOP 8 SLICES and press A. Y auditions at the original sample root.
- GRANULAR is its own ninth page. Select it, load a WAV with X (or A when empty),
  or press A to enable granular on the existing sample. Position, grain size,
  shape, spread, direction, octave, LFO and five anchors are adjustable.
- FX has five tabs: Reverb, Delay, Slicer, EQ, Sends. L1/R1 changes tabs;
  L2+L1/R1 changes the selected track. Arrows select and adjust parameters.
- The sample engine accepts PCM 8/16/24/32-bit and IEEE floating-point WAVs.
  Stereo sources are downmixed to mono to fit the existing sampler engine.
- Update-Knulli.command builds, runs ARM64 regression tests, compares the
  copied binary, and keeps a backup. It never overwrites projects, samples
  or the physical controller mapping.

## Knulli next step

Knulli Scarab 2026/05/11, SHARE partition, PortMaster and ARM64 ChooChooTracker
were inspected. Version 0.2 is installed under SHARE/roms/ports/terminpocket.
Mac preview is Mach-O, **not the Linux ELF executable** required by Knulli.

Build the source in an ARM64 Linux environment (PortMaster recommends its
build environment). Required: C++17 compiler, CMake, pkg-config, SDL2 and
cJSON development headers. Run Build-Linux.sh in that environment.
It creates port/terminpocket/terminpocket.aarch64. Check target glibc and
dependencies, SDL gamepad mappings, audio driver, underruns and CPU load
on the real RG35XX Plus. Only then copy TerminPocket.sh, TerminPocket.png,
and terminpocket/ to SHARE/roms/ports. Do not copy the incomplete launcher
to the card before the Linux binary exists.

Next features after device validation: USB MIDI, live input recording,
full Creative Fill, lossless desktop project roundtrip, time stretch,
native WAV export, optimized waveform cache and device performance tuning.
