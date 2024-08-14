# YourGameLib development Docker image
#
# Build full image
#   docker build -t yourgame .
# Build image without DOCS/EMSDK/ANDROID_SDK
#   docker build -t yourgame_small --build-arg NO_DOCS="true" --build-arg NO_EMSDK="true" --build-arg NO_ANDROID="true" .
# Control Ubuntu version
#   --build-arg UBUNTU_VERSION=<version>
#
# Start self-deleting interactive container from image, mounting
# current working directory to /code:
# PowerShell
#   docker run --rm -it -p 8000:8000 -v ${PWD}:/code yourgame
# bash
#   docker run --rm -it -p 8000:8000 -v $(pwd):/code yourgame
ARG UBUNTU_VERSION="latest"
FROM ubuntu:${UBUNTU_VERSION}

ARG NO_DOCS="false"
ARG NO_EMSDK="false"
ARG NO_ANDROID="false"

ENV DEBIAN_FRONTEND=noninteractive

RUN apt-get update && apt-get install -y \
    git \
    make \
    gcc \
    g++ \
    cmake \
    python3 \
    python3-pip \
    zip \
    libgl1-mesa-dev \
    wget \
    xorg-dev && \
    rm -rf /var/lib/apt/lists/*

# Install tools for docs and quality
RUN if [ "$NO_DOCS" = "false" ]; then \
      apt-get update && apt-get install -y \
      clang-format \
      clang-tidy \
      doxygen \
      graphviz \
      plantuml && \
      rm -rf /var/lib/apt/lists/*; \
    fi

RUN ln -s /usr/bin/python3 /usr/bin/python
# /usr/bin/pip -> /usr/bin/pip3 is already present

# Install Python requirements for project init and docs
COPY init/requirements.txt /tmp/requirements_init.txt
COPY docs/requirements.txt /tmp/requirements_docs.txt
RUN PIP_BREAK_SYSTEM_PACKAGES=1 pip install -r /tmp/requirements_init.txt
RUN if [ "$NO_DOCS" = "false" ]; then \
      PIP_BREAK_SYSTEM_PACKAGES=1 pip install -r /tmp/requirements_docs.txt; \
    fi

# Install emsdk
RUN if [ "$NO_EMSDK" = "false" ]; then \
      cd /opt && \
      git clone https://github.com/emscripten-core/emsdk.git && \
      cd emsdk && \
      ./emsdk install latest && \
      ./emsdk activate latest; \
    fi

# Set EMSDK location to call
#   source $EMSDK/emsdk_env.sh
# via build scripts
ENV EMSDK=/opt/emsdk

# Install OpenJDK (for Android Gradle builds)
RUN if [ "$NO_ANDROID" = "false" ]; then \
      apt-get update && apt-get install -y \
      openjdk-21-jre && \
      rm -rf /var/lib/apt/lists/*; \
    fi

# Install Android SDK
RUN if [ "$NO_ANDROID" = "false" ]; then \
      cd /opt && \
      wget https://dl.google.com/android/repository/commandlinetools-linux-12172612_latest.zip && \
      unzip commandlinetools-linux-12172612_latest.zip -d android_sdk && \
      cd android_sdk/cmdline-tools && \
      mkdir latest && \
      mv NOTICE.txt latest/ && \
      mv bin latest/ && \
      mv lib latest/ && \
      mv source.properties latest/ && \
      yes | /opt/android_sdk/cmdline-tools/latest/bin/sdkmanager --licenses; \
    fi

ENV ANDROID_HOME=/opt/android_sdk/cmdline-tools/latest/

# Install Gradle
RUN if [ "$NO_ANDROID" = "false" ]; then \
      cd /opt && \
      wget https://services.gradle.org/distributions/gradle-8.9-bin.zip && \
      unzip gradle-8.9-bin.zip -d gradle; \
    fi

ENV PATH="$PATH:/opt/gradle/gradle-8.9/bin"

# ToDo: Android builds fail due to missing NDK license
# Checking the license for package NDK (Side by side) 25.2.9519653 in /opt/android_sdk/cmdline-tools/latest/licenses
# Warning: License for package NDK (Side by side) 25.2.9519653 not accepted.

WORKDIR /code

# Assuming /code is mounted
RUN git config --global --add safe.directory /code

EXPOSE 8000
