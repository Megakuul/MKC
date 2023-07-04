#!/bin/bash

mkdir build
cd build

cmake ..

make

cp xmdr ../xmdr/usr/bin/