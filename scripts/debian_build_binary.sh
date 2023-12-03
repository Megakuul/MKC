#!/bin/bash

makedeps=("clang" "make" "cmake" "dpkg-deb" "dpkg-query" "git" "sed")
libdeps=("libgtkmm-3.0-dev" "zlib1g-dev")

# Check if make dependencies are on the system
for dep in "${makedeps[@]}"; do
	if command -v "$dep" >/dev/null 2>&1; then
		continue
	else
		echo "Build dependency $dep is missing or noth in PATH"
		exit 1
	fi
done

# Check if dev libraries are on the system
for ldep in "${libdeps[@]}"; do
	if dpkg-query -W -f='${Status}' "$ldep" 2>/dev/null | grep -q "install ok installed"; then
		continue
	else
		echo "Dev library $ldep is missing"
		exit 1
	fi
done

cd ..

set -e

# Build the project
mkdir -p build
cd build
cmake ..
make
# Install files into the virtual directory
make DESTDIR="../DEBIAN" install

cd ../..
# Build the .dep package
dpkg-deb --build MKC

