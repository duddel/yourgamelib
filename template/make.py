import sys
import os
import argparse
import subprocess


def clean():
    raise NotImplementedError


def build_desktop():
    projRoot = os.path.dirname(__file__)
    projBuild = os.path.join(projRoot, "_build")
    if not os.path.isdir(projBuild):
        os.mkdir(projBuild)
        confCmd = ["cmake", "..", "-DYOURGAME_PLATFORM=desktop"]
        if args.release:
            confCmd.append("-DCMAKE_BUILD_TYPE=RELEASE")
        else:
            confCmd.append("-DCMAKE_BUILD_TYPE=DEBUG")
        if args.generator:
            confCmd.extend(["-G", args.generator])
        print("configure command: {}".format(" ".join(confCmd)))
        procRes = subprocess.run(confCmd, cwd=projBuild)
    buildCmd = ["cmake", "--build", "."]
    print("build command: {}".format(" ".join(buildCmd)))
    procRes = subprocess.run(buildCmd, cwd=projBuild)


def build_android():
    projRoot = os.path.dirname(__file__)
    projBuild = os.path.join(projRoot, "android")
    if args.release:
        buildCmd = ["./gradlew", "assembleRelease"]
    else:
        buildCmd = ["./gradlew", "assembleDebug"]
    print("build command: {}".format(" ".join(buildCmd)))
    procRes = subprocess.run(buildCmd, cwd=projBuild)


def build_web():
    projRoot = os.path.dirname(__file__)
    projBuild = os.path.join(projRoot, "_build_web")
    if not os.path.isdir(projBuild):
        os.mkdir(projBuild)
        # call source $EMSDK/emsdk_env.sh BEFORE
        confCmd = ["emconfigure", "cmake", "..", "-DYOURGAME_PLATFORM=wasm"]
        if args.release:
            confCmd.append("-DCMAKE_BUILD_TYPE=RELEASE")
        else:
            confCmd.append("-DCMAKE_BUILD_TYPE=DEBUG")
        if args.generator:
            confCmd.extend(["-G", args.generator])
        print("configure command: {}".format(" ".join(confCmd)))
        procRes = subprocess.run(confCmd, cwd=projBuild)
    buildCmd = ["cmake", "--build", "."]
    print("build command: {}".format(" ".join(buildCmd)))
    procRes = subprocess.run(buildCmd, cwd=projBuild)


argPars = argparse.ArgumentParser()
argPars.add_argument("-d", "--desktop", action="store_true",
                     help="configure and build, target desktop")
argPars.add_argument("-a", "--android", action="store_true",
                     help="configure and build, target android")
argPars.add_argument("-w", "--web", action="store_true",
                     help="configure and build, target web (WebAssembly)")
argPars.add_argument("--release", action="store_true",
                     help="build release, default debug")
argPars.add_argument(
    "--generator", type=str, help="only used for -d, passed to CMake as generator (-G)")
argPars.add_argument("--clean", action="store_true",
                     help="delete _build*/ and exit")
args = argPars.parse_args()

if len(sys.argv) == 1:
    argPars.print_help()
    sys.exit(1)

if args.clean:
    clean()
    sys.exit(0)

if args.desktop:
    build_desktop()

if args.android:
    build_android()

if args.web:
    build_web()
