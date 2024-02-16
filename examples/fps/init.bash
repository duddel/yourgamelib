#!/bin/bash

cd "${0%/*}"
cd ..

python ../init/init.py fps --noStub --extProj bullet miniaudio
