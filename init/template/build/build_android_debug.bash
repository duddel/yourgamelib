#!/bin/bash

cd android
gradle assembleDebug --stacktrace || exit $?
cd -
