@echo off

cd %~dp0
cd ..

python ../init/init.py framework --noTemplate --extProj box2d lua miniaudio
