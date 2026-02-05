# Backwards-compatible entrypoint for non-flake Nix users.
let
  flake = builtins.getFlake (toString ./.);
in
  flake.devShells.${builtins.currentSystem}.default
