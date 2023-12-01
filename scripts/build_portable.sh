#!/bin/bash

cd ..

mkdir -p build
cd build

set -e

cmake ..

make

echo "Make sure that the target system has gtkmm3.0 and zlib installed" > README.txt

tar -czvf ../mkc_portable.tar.gz mkc README.txt

rm -f README.txt
