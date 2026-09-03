.PHONY: init build clear init-windows build-windows clear-windows

# Linux targets
init:
	cmake -B build -G Ninja

build:
	cmake --build build

clear:
	rm -rf build

# Windows cross-compilation targets
init-windows:
	cmake -B build-windows -G Ninja -DCMAKE_TOOLCHAIN_FILE=cmake/windows-toolchain.cmake

build-windows:
	cmake --build build-windows

clear-windows:
	rm -rf build-windows
