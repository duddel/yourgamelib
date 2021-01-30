@echo off

cd %~dp0
cd ..

python ../init.py fps --noSources --extProj bullet
