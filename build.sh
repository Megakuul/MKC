#!/bin/bash

unset GTK_PATH

mkdir -p build
cd build

cmake ..

make

cp xmdr ../xmdr/usr/bin/