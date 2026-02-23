{
  description = "CrossPoint Reader - ESP32-C3 E-Paper Firmware Development Environment";

  inputs = {
    nixpkgs.url = "github:NixOS/nixpkgs/nixos-unstable";
    flake-utils.url = "github:numtide/flake-utils";
  };

  outputs = { self, nixpkgs, flake-utils }:
    flake-utils.lib.eachDefaultSystem (system:
      let
        pkgs = import nixpkgs { inherit system; };

        pythonEnv = pkgs.python311.withPackages (ps: with ps; [
          pyserial
          colorama
          matplotlib
          pillow
        ]);
      in
      {
        devShells.default = pkgs.mkShell {
          buildInputs = with pkgs; [
            # Core build tools
            platformio
            pythonEnv
            git

            # Code formatting (matching CI version)
            llvmPackages_21.clang-tools

            # USB device access
            usbutils
          ];
        };
      }
    );
}
