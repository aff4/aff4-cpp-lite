{
  description = "AFF4 C++ Light forensic container library";

  inputs = {
    nixpkgs.url = "github:NixOS/nixpkgs/nixos-unstable";
    flake-parts.url = "github:hercules-ci/flake-parts";
  };

  outputs = inputs@{ flake-parts, nixpkgs, ... }:
    flake-parts.lib.mkFlake { inherit inputs; } {
      systems = [
        "x86_64-linux"
        "aarch64-linux"
        "x86_64-darwin"
        "aarch64-darwin"
      ];

      perSystem = { pkgs, ... }:
        let
          raptor2Pkg = if pkgs ? raptor2 then pkgs.raptor2 else pkgs.librdf;
        in
        {
          packages.default = pkgs.callPackage ./nix/aff4-cpp-lite.nix {
            raptor2 = raptor2Pkg;
          };
        };
    };
}
