#!/bin/bash

cd "${0%/*}"
cd ..

python3 ../init.py framework --noStub --extProj box2d flecs lua choreograph fsm qu3e
