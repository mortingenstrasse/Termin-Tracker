#!/bin/bash
# Install only after terminpocket.aarch64 has been compiled and tested.
GAMEDIR="/userdata/roms/ports/terminpocket"
controlfolder="/userdata/roms/ports/PortMaster"
if [ -f "$controlfolder/control.txt" ]; then
    source "$controlfolder/control.txt"
    [ -f "$controlfolder/mod_${CFW_NAME}.txt" ] && source "$controlfolder/mod_${CFW_NAME}.txt"
    get_controls
    export SDL_GAMECONTROLLERCONFIG="${sdl_controllerconfig:-}"
fi
cd "$GAMEDIR" || exit 1
binary="$GAMEDIR/terminpocket.aarch64"
if [ ! -f "$binary" ]; then
    if type pm_message >/dev/null 2>&1; then pm_message "Termin Pocket ARM64 build missing"; fi
    echo "ARM64 build missing" > "$GAMEDIR/log.txt"
    exit 1
fi
export TERMIN_POCKET_HANDHELD=1
export LD_LIBRARY_PATH="$GAMEDIR/lib${LD_LIBRARY_PATH:+:$LD_LIBRARY_PATH}"
chmod +x "$binary"
if type pm_platform_helper >/dev/null 2>&1; then pm_platform_helper "$binary"; fi
"$binary" "$GAMEDIR" > "$GAMEDIR/log.txt" 2>&1
result=$?
if type pm_finish >/dev/null 2>&1; then pm_finish; fi
exit "$result"
