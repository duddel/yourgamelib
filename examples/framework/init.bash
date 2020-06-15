#!/bin/bash

cd "${0%/*}"
cd ..

python ../tools/init.py framework level2 --noSources --extProj imgui
