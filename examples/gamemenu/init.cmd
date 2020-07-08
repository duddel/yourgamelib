@echo off

cd %~dp0
cd ..

python ../tools/init.py GameMenu level2 --noSources --extProj imgui
