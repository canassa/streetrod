#!/usr/bin/env bash
# Interactive DOSBox-X with the MSC 5.1 toolchain on D: and the project on C:.
#
#   ./dosbox.sh            C: = project root, D: = toolchain/msc51
set -euo pipefail

HERE="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
mkdir -p "$HERE/work/TMP"

exec dosbox-x \
    -c "mount c \"$HERE\"" \
    -c "mount d \"$HERE/toolchain/msc51\"" \
    -c "set PATH=D:\\BIN" \
    -c "set INCLUDE=D:\\INCLUDE" \
    -c "set LIB=D:\\LIB" \
    -c "set TMP=C:\\WORK\\TMP" \
    -c "c:" \
    "$@"
