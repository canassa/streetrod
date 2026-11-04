{
  description = "Street Rod (1989) reverse engineering with Microsoft C 5.1";

  inputs = {
    nixpkgs.url = "github:NixOS/nixpkgs/nixos-unstable";
    flake-utils.url = "github:numtide/flake-utils";
  };

  outputs = { self, nixpkgs, flake-utils }:
    flake-utils.lib.eachDefaultSystem (system:
      let
        pkgs = import nixpkgs { inherit system; };
      in {
        devShells.default = pkgs.mkShell {
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
        };
      });
}
