@echo off

cd %~dp0
cd ..

python ../init.py framework --noStub --extProj box2d flecs lua choreograph fsm qu3e
