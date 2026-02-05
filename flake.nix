{
  description = "AFF4 C++ Light forensic container library";

  inputs = {
    nixpkgs.url = "github:NixOS/nixpkgs/nixos-25.11";
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
          raptor2Pkg = pkgs.lib.attrByPath [ "raptor2" ] null pkgs;
          librdfPkg = pkgs.lib.attrByPath [ "librdf" ] null pkgs;
          redlandPkg = pkgs.lib.attrByPath [ "redland" ] null pkgs;
          selectedRaptor2 =
            if raptor2Pkg != null then raptor2Pkg
            else if librdfPkg != null then librdfPkg
            else if redlandPkg != null then redlandPkg
            else throw "No raptor2/librdf/redland package available for this system.";
        in
        {
          packages.default = pkgs.callPackage ./nix/aff4-cpp-lite.nix {
            raptor2 = selectedRaptor2;
          };
        };
    };
}
