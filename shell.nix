{ pkgs ? import <nixpkgs> { } }:
pkgs.mkShell {
  packages = with pkgs; [
    dosbox-x
    gnumake
    (callPackage ./tools/nix/emu2.nix { })  # default compile backend, see tools/sr/emu2.py
    (python3.withPackages (p: [ p.capstone ]))
    nasm
    mtools
    p7zip
    sqlite
  ];
}
