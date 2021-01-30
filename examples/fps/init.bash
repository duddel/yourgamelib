#!/bin/bash

cd "${0%/*}"
cd ..

python ../init.py fps --noSources --extProj bullet
