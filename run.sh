#!/usr/bin/env bash
set -e

cmake -S . -B build
cmake --build build -j8

cd build
./FoCalESimulation
