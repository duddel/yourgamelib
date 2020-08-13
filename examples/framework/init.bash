#!/bin/bash

cd "${0%/*}"
cd ..

python ../tools/init.py framework --noSources --extProj box2d
