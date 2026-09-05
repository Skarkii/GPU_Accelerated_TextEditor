{
  description = "GPU-accelerated text editor development environment";

  inputs = {
    nixpkgs.url = "github:NixOS/nixpkgs/nixos-unstable";
    flake-utils.url = "github:numtide/flake-utils";
  };

  outputs = { self, nixpkgs, flake-utils }:
    flake-utils.lib.eachDefaultSystem (system:
      let
        pkgs = import nixpkgs { inherit system; };

        # Common dependencies for both native and cross builds
        commonBuildInputs = with pkgs; [
          # Vulkan
          vulkan-headers
          vulkan-loader
          vulkan-validation-layers

          # Windowing
          glfw

          # Math library
          glm

          # Shader compilation
          shaderc
          glslang

          # Testing
          catch2_3
        ];

        commonNativeBuildInputs = with pkgs; [
          # Build tools
          cmake
          ninja
          pkg-config

          # C++ compiler
          gcc13
        ];

      in
      {
        devShells = {
          # Native Linux development shell
          default = pkgs.mkShell {
            name = "gpu-text-editor-dev";

            nativeBuildInputs = commonNativeBuildInputs;

            buildInputs = commonBuildInputs ++ (with pkgs; [
              # Linux-specific
              xorg.libX11
              xorg.libXrandr
              xorg.libXinerama
              xorg.libXcursor
              xorg.libXi
              wayland
              wayland-protocols
              libxkbcommon
            ]);

            shellHook = ''
              echo "GPU Text Editor Development Environment"
              echo "======================================="
              echo "Vulkan SDK ready"
              echo ""
              echo "Build commands:"
              echo "  cmake -B build -G Ninja"
              echo "  cmake --build build"
              echo ""
              export VK_LAYER_PATH="${pkgs.vulkan-validation-layers}/share/vulkan/explicit_layer.d"
            '';

            # Ensure Vulkan ICD is found
            VK_ICD_FILENAMES = "${pkgs.vulkan-loader}/share/vulkan/icd.d/nvidia_icd.json";
          };

          # Windows cross-compilation shell
          windows = let
            mingw = pkgs.pkgsCross.mingwW64;
          in pkgs.mkShell {
            name = "gpu-text-editor-windows";

            nativeBuildInputs = with pkgs; [
              cmake
              ninja
            ];

            buildInputs = [
              mingw.stdenv.cc
              mingw.windows.pthreads
            ];

            shellHook = ''
              echo "Windows Cross-Compilation Environment"
              echo "======================================"
              echo ""
              echo "Dependencies in external/:"
              echo "  - Vulkan-Headers (clone from GitHub)"
              echo "  - glfw (clone from GitHub)"
              echo "  - glm (clone from GitHub)"
              echo ""
              echo "Build command:"
              echo "  cmake -B build-windows -G Ninja -DCMAKE_TOOLCHAIN_FILE=cmake/windows-toolchain.cmake"
              echo "  cmake --build build-windows"
              echo ""
              echo "Output: build-windows/gpu_text_editor.exe"
            '';
          };
        };

        # Package definition for building the editor
        packages.default = pkgs.stdenv.mkDerivation {
          pname = "gpu-text-editor";
          version = "0.1.0";

          src = ./.;

          nativeBuildInputs = commonNativeBuildInputs;
          buildInputs = commonBuildInputs ++ (with pkgs; [
            xorg.libX11
            xorg.libXrandr
            xorg.libXinerama
            xorg.libXcursor
            xorg.libXi
            wayland
            wayland-protocols
            libxkbcommon
          ]);

          configurePhase = ''
            cmake -B build -G Ninja -DCMAKE_BUILD_TYPE=Release
          '';

          buildPhase = ''
            cmake --build build
          '';

          installPhase = ''
            mkdir -p $out/bin
            cp build/gpu_text_editor $out/bin/
          '';
        };
      }
    );
}
