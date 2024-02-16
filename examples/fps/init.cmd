@echo off

cd %~dp0
cd ..

python ../init/init.py fps --noStub --extProj bullet miniaudio
