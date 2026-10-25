#!/usr/bin/env bash

emcmake cmake .. -B wasm -DSPIKY_NES_BIND_ES=ON -DCMAKE_BUILD_TYPE=Release
(cd wasm && emmake make SpikyNesWASM)
