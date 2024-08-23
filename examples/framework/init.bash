#!/bin/bash

cd "${0%/*}"
cd ..

python3 ../init/init.py framework --noTemplate --extProj box2d lua miniaudio
