#!/bin/bash

mkdir -p build
cd build

cmake ..

make

cp xmdr ../xmdr/usr/bin/