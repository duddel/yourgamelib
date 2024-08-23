#!/bin/bash

cd "${0%/*}"
cd ..

python ../init/init.py particles --noTemplate
