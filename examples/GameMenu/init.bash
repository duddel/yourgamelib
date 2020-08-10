#!/bin/bash

cd "${0%/*}"
cd ..

python ../tools/init.py GameMenu --noSources --extProj imgui
