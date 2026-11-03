# emu2 (github.com/dmsc/emu2), the default ./mc compile backend (tools/mc/emu2.py).
# Pinned to upstream master (nixpkgs still ships a 2021 snapshot, whose MASM listings come
# out truncated), plus one fix: flush the parent's buffered DOS output before spawning a
# child, so CL's "FILE.C" line lands in CC.LOG before C1's warnings, as under DOS.
{ emu2, fetchFromGitHub }:
emu2.overrideAttrs (old: {
  version = "2026.04.26-9d8698d";
  src = fetchFromGitHub {
    owner = "dmsc";
    repo = "emu2";
    rev = "9d8698d06e67359bc8b230c2d19d5b1503e60819";
    sha256 = "1lrg3gqvcvwj09mpv2ks2ajia2a20jd0npn2xlad5w9g480dvabg";
  };
  patches = (old.patches or [ ]) ++ [ ./emu2-flush-before-fork.patch ];
})
