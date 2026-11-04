{ pkgs ? import <nixpkgs> { } }:
pkgs.mkShell {
  packages = with pkgs; [
    dosbox-x
    gnumake
    doxygen
    graphviz
    (callPackage ./tools/nix/emu2.nix { })  # default compile backend, see tools/sr/emu2.py
    (python3.withPackages (p: [ p.capstone ]))
    p7zip  # unpacks the .7z archives (and the FAT disk image) in dist/ and toolchain/*/dist/
  ];
}
