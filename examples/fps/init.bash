#!/bin/bash

cd "${0%/*}"
cd ..

python ../init/init.py fps --noTemplate --extProj bullet miniaudio
