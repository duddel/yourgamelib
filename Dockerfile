# This Dockerfile can be used to build an image for building
# yourgame projects on Ubuntu/web. WORK IN PROGRESS, though.
#
# Build the image:
#   docker build -t yourgame .
#
# Start self-deleting interactive container from image, mounting
# current working directory to /code:
#
# Powershell
#   docker run --rm -it -p 8000:8000 -v ${PWD}:/code yourgame
# bash
#   docker run --rm -it -p 8000:8000 -v $(pwd):/code yourgame
FROM ubuntu:22.04

ENV DEBIAN_FRONTEND=noninteractive

RUN apt-get update && apt-get install -y \
    git \
    make \
    gcc \
    g++ \
    cmake \
    python3 \
    python3-pip \
    libgl1-mesa-dev \
    xorg-dev && \
    rm -rf /var/lib/apt/lists/*

RUN ln -s /usr/bin/python3 /usr/bin/python
# /usr/bin/pip -> /usr/bin/pip3 is already present

COPY requirements.txt /tmp

RUN python -m pip install --upgrade pip &&\
    pip install -r /tmp/requirements.txt

# install emsdk
RUN git clone https://github.com/emscripten-core/emsdk.git &&\
    cd emsdk &&\
    ./emsdk install latest &&\
    ./emsdk activate latest

# set EMSDK location to call
#   source $EMSDK/emsdk_env.sh
# via build scripts
ENV EMSDK=/emsdk

# assuming /code is mounted
RUN git config --global --add safe.directory /code

EXPOSE 8000
