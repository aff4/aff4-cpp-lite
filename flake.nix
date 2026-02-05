{
  description = "Nix build/dev environment for aff4-cpp-lite";

  inputs = {
    nixpkgs.url = "github:NixOS/nixpkgs/nixos-24.05";
    flake-utils.url = "github:numtide/flake-utils";
  };

  outputs = { self, nixpkgs, flake-utils }:
    flake-utils.lib.eachDefaultSystem (system:
      let
        pkgs = import nixpkgs { inherit system; };

        raptor2Pkg = if pkgs ? raptor2 then pkgs.raptor2 else pkgs.librdf_raptor2;

        aff4 = pkgs.stdenv.mkDerivation {
          pname = "aff4-cpp-lite";
          version = "2.0.6";
          src = self;

          nativeBuildInputs = with pkgs; [
            autoreconfHook
            pkg-config
            libtool
            autoconf
            automake
            doxygen
          ];

          buildInputs = with pkgs; [
            zlib
            snappy
            lz4
            raptor2Pkg
            cppunit
            openssl
          ];

          configureFlags = [
            "--enable-debug=no"
          ];

          doCheck = true;

          meta = with pkgs.lib; {
            description = "Lightweight C++11 implementation of AFF4";
            homepage = "https://github.com/Velocidex/c-aff4";
            license = licenses.lgpl3Plus;
            platforms = platforms.unix;
          };
        };
      in {
        packages = {
          default = aff4;
          aff4-cpp-lite = aff4;
        };

        devShells.default = pkgs.mkShell {
          inputsFrom = [ aff4 ];
          packages = with pkgs; [
            gdb
            clang-tools
          ];
        };
      });
}
