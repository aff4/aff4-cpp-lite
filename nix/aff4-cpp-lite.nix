{ lib
, stdenv
, autoreconfHook
, pkg-config
, libtool
, zlib
, lz4
, raptor2
, snappy
}:

stdenv.mkDerivation {
  pname = "aff4-cpp-lite";
  version = "2.0.6";

  src = lib.cleanSource ../.;

  nativeBuildInputs = [
    autoreconfHook
    pkg-config
    libtool
  ];

  buildInputs = [
    zlib
    lz4
    raptor2
    snappy
  ];

  configureFlags = [ "--disable-debug" ];

  doCheck = false;

  meta = with lib; {
    description = "AFF4 C++ Light forensic container library";
    homepage = "https://github.com/evimetry/aff4-cpp";
    license = licenses.lgpl3Plus;
    platforms = platforms.unix;
  };
}
