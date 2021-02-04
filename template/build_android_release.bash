#!/bin/bash

if [ ! -d "_deploy" ]; then
  mkdir _deploy
fi

cd android
./gradlew assembleRelease
cd -
mv -f android/app/build/outputs/apk/release/*.apk _deploy/
