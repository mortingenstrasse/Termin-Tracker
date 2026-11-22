#!/bin/bash
# Run in an ARM64 Linux/PortMaster development environment, not in macOS.
set -euo pipefail
cd "$(dirname "$0")"
if [[ "$(uname -s)" != Linux || "$(uname -m)" != aarch64 ]]; then
  echo "Run in an ARM64 Linux build environment with SDL2 and cJSON development libraries." >&2
  exit 1
fi
cmake -S Source -B build-linux -DCMAKE_BUILD_TYPE=Release
cmake --build build-linux -j2
python3 Generate-Test-Wavs.py build-linux/terminpocket
cp build-linux/terminpocket port/terminpocket/terminpocket.aarch64
mkdir -p port/terminpocket/lib
cjson_path=$(ldd build-linux/terminpocket | awk '/libcjson\.so/ && $3 ~ /^\// {print $3; exit}')
if [[ -n "$cjson_path" ]]; then
  cp -L "$cjson_path" "port/terminpocket/lib/$(basename "$cjson_path")"
fi
chmod +x port/TerminPocket.sh port/terminpocket/terminpocket.aarch64
file port/terminpocket/terminpocket.aarch64
ldd port/terminpocket/terminpocket.aarch64
echo "ARM64 build complete. Verify libraries and audio/gamepad on Knulli before release."
