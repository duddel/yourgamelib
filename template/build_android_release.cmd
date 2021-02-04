@echo off

IF NOT EXIST _deploy\ (
  mkdir _deploy
)

cd android && gradlew assembleRelease && cd .. && move /Y android\app\build\outputs\apk\release\*.apk _deploy
