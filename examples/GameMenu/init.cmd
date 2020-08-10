@echo off

cd %~dp0
cd ..

python ../tools/init.py GameMenu --noSources --extProj imgui
