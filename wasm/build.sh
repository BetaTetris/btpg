#!/bin/bash


cd -- "$( dirname -- "${BASH_SOURCE[0]}" )" &> /dev/null
git submodule update --init

cd ../emsdk
EMSDK_VER=5.0.5
./emsdk activate $EMSDK_VER || ( ./emsdk install $EMSDK_VER && ./emsdk activate $EMSDK_VER )
source ./emsdk_env.sh

cd ../wasm
emcc.py -O2 -std=c++20 -fexceptions -sALLOW_MEMORY_GROWTH -sWASM_BIGINT -sENVIRONMENT=web -sEXPORT_ES6 \
    -o tetris.js --emit-tsd tetris.d.ts --emit-symbol-map \
    tetris.cpp binding/*.cpp tetris/frame_sequence.cpp -lembind

# emcc -O2 -std=c++20 -fexceptions -sALLOW_MEMORY_GROWTH -sWASM_BIGINT -sENVIRONMENT=web -sSINGLE_FILE \
#     -o tetris-single.js \
#     tetris.cpp binding/*.cpp tetris/frame_sequence.cpp -lembind

# -s'EXPORT_NAME="TetrisModule"'
