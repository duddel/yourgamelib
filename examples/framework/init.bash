#!/bin/bash

cd "${0%/*}"
cd ..

python3 ../tools/init.py framework --noSources --extProj box2d flecs lua choreograph fsm
