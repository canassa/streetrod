#!/usr/bin/env bash
# The DOS-native build: DOSBox-X with the drives src/MAKEFILE expects, and MSC SETUP's
# environment. C: is work/dos, a fresh copy of src/ (MAKEFILE, SR.LNK, SRENV.BAT and subsystem folders); D: toolchain/msc51; F: toolchain/masm51. No host tool is involved.
#   ./dosbox.sh          interactive: SRENV.BAT is called before the prompt; type MAKE MAKEFILE
#   ./dosbox.sh --make   headless: MAKE MAKEFILE > MAKE.LOG, then compare C:\SR.EXE with orig/SR.EXE
set -euo pipefail

HERE="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
command -v dosbox-x >/dev/null || exec nix-shell "$HERE/shell.nix" --run "$(printf '%q ' "$0" "$@")"
TREE="$HERE/work/dos"
rm -rf "$TREE"
mkdir -p "$TREE/TMP"
cp -R "$HERE/src/." "$TREE/"

args=(-c "mount c \"$TREE\"" -c "mount d \"$HERE/toolchain/msc51\"" -c "mount f \"$HERE/toolchain/masm51\""
      -c "set PATH=D:\\BIN;F:\\BIN" -c "set LIB=D:\\LIB"
      -c "set TMP=C:\\TMP" -c "c:" -c "call SRENV.BAT")
if [ "${1:-}" != "--make" ]; then
    exec dosbox-x "${args[@]}" "$@"
fi
# bounded like every DOS run here: a time limit and a file-size limit for the redirected log
( ulimit -f 200000
  SDL_VIDEODRIVER=dummy timeout 600 dosbox-x -silent -fastlaunch -nogui -exit \
      -set "quit warning=false" -set core=dynamic_x86 -set cycles=max \
      "${args[@]}" -c "MAKE MAKEFILE > MAKE.LOG" -c exit > "$TREE/DOSBOX.OUT" 2>&1 )
[ -f "$TREE/SR.EXE" ] || { tail -20 "$TREE/MAKE.LOG"; echo "MAKE did not build SR.EXE" >&2; exit 1; }
sha1sum "$TREE/SR.EXE" "$HERE/orig/SR.EXE"
cmp -s "$TREE/SR.EXE" "$HERE/orig/SR.EXE" && echo "DOS-native SR.EXE vs orig/SR.EXE: IDENTICAL" \
    || { echo "DOS-native SR.EXE vs orig/SR.EXE: DIFFERENT"; exit 1; }
