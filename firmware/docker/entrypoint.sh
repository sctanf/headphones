#!/bin/bash
cd /headphones/firmware/docker
mkdir -p build/code
mkdir -p build/tools

pushd .
cd build/code
PICO_SDK_FETCH_FROM_GIT=1 PICO_EXTRAS_FETCH_FROM_GIT=1 cmake ../../../code
make -j `nproc`
popd

pushd .
cd build/tools
cmake ../../../tools
make -j `nproc`
popd