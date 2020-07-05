@echo off

cd %~dp0
cd ..

python ../tools/init.py gamemenu level2 --noSources --extProj imgui
