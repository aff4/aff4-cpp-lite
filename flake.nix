{
  description = "AFF4 C++ Lite (proper Nix package)";

  inputs = {
    nixpkgs.url = "github:NixOS/nixpkgs/nixos-24.05";
    flake-utils.url = "github:numtide/flake-utils";
  };

  outputs = { self, nixpkgs, flake-utils }:
    flake-utils.lib.eachDefaultSystem (system:

      let
        pkgs = import nixpkgs { inherit system; };

        raptor2Pkg =
          if pkgs ? raptor2 then pkgs.raptor2 else pkgs.librdf_raptor2;

      in {

        packages.default =
          pkgs.stdenv.mkDerivation {
            pname = "aff4-cpp-lite";
            version = "2.0.6";

            src = self;

            nativeBuildInputs = with pkgs; [
              autoreconfHook
              pkg-config
              autoconf
              automake
              libtool
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
              "--enable-static"
              "--enable-shared"
            ];

            enableParallelBuilding = true;

            doCheck = false;

            meta = with pkgs.lib; {
              description = "AFF4 C++ implementation";
              license = licenses.lgpl3Plus;
              platforms = platforms.unix;
            };
          };

      });
}
