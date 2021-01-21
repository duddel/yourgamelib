@echo off

cd %~dp0
cd ..

python ../init.py framework --noSources --extProj box2d flecs lua choreograph fsm qu3e bullet
