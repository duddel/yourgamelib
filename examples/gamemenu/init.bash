#!/bin/bash

cd "${0%/*}"
cd ..

python ../tools/init.py GameMenu level2 --noSources --extProj imgui
