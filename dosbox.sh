#!/usr/bin/env bash
# Interactive DOSBox-X set up for the DOS-native build (prepare it with ./sr build --prepare-only):
#   C: = work/layout, D: = toolchain/msc51, E: = work/layout/INCLUDE, F: = toolchain/masm51
# then MAKE MAKEFILE (or BUILD.BAT) builds C:\SR.EXE.
set -euo pipefail

HERE="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
TREE="$HERE/work/layout"
[ -f "$TREE/MAKEFILE" ] || { echo "no $TREE/MAKEFILE: run ./sr build --prepare-only first" >&2; exit 1; }

exec dosbox-x \
    -c "mount c \"$TREE\"" \
    -c "mount d \"$HERE/toolchain/msc51\"" \
    -c "mount e \"$TREE/INCLUDE\"" \
    -c "mount f \"$HERE/toolchain/masm51\"" \
    -c "c:" \
    -c "ENV.BAT" \
    "$@"
