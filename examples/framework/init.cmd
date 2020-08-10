@echo off

cd %~dp0
cd ..

python ../tools/init.py framework --noSources --extProj imgui json box2d
