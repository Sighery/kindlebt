{ pkgs ? import <nixpkgs> {} }:

pkgs.mkShell {
  buildInputs = with pkgs; [
    meson ninja pkg-config python3
    bintools patchelf libdwarf
    gnutar
    clang-tools
    gdb unixtools.xxd
  ];
}
