{ pkgs ? import <nixpkgs> { } }:
pkgs.mkShell {
  packages = with pkgs; [
    dosbox-x
    (python3.withPackages (p: [ p.capstone ]))
    nasm
    mtools
    p7zip
    sqlite
  ];
}
