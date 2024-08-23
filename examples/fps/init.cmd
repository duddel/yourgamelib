@echo off

cd %~dp0
cd ..

python ../init/init.py fps --noTemplate --extProj bullet miniaudio
