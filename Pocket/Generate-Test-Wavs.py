#!/usr/bin/env python3
"""Generate temporary synthetic WAVs for the Pocket regression suite; no sample pack is distributed."""
import math, pathlib, subprocess, sys, tempfile, wave

with tempfile.TemporaryDirectory(prefix="termin-test-") as directory:
    root = pathlib.Path(directory)
    for category in ("01 Kick", "02 Snare", "03 HiHat", "04 Percussion", "05 Bass", "06 Synth"):
        folder = root / "Sample Set" / category
        folder.mkdir(parents=True)
        for i in range(10):
            width = 2 if i % 2 == 0 else 3
            channels = 1 if i % 3 == 0 else 2
            sr = 44100
            frames = bytearray()
            for n in range(sr // 3):
                x = n / sr
                tone = 0.45 * math.sin(2 * math.pi * (110 + i * 13) * x) * (1 - x * 2)
                value = int(tone * (2 ** (width * 8 - 1) - 1))
                frames.extend(value.to_bytes(width, "little", signed=True) * channels)
            with wave.open(str(folder / f"{i+1:02d} Synthetic_C4_test.wav"), "wb") as w:
                w.setnchannels(channels); w.setsampwidth(width); w.setframerate(sr); w.writeframes(frames)
    subprocess.run([sys.argv[1], "--self-test", str(root)], check=True)
