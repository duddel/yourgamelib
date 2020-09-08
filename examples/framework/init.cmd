@echo off

cd %~dp0
cd ..

python ../tools/init.py framework --noSources --extProj box2d flecs lua
