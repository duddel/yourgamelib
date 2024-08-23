import os
import posixpath
import argparse
import shutil
import glob
from jinja2 import Environment, FileSystemLoader
import git


def copy_file_items(items, src, dst):
    for item in items:
        item_src = os.path.join(src, item)
        item_dst = os.path.join(dst, item)
        if os.path.isdir(item_src):
            shutil.copytree(item_src, item_dst, dirs_exist_ok=True)
        else:
            shutil.copy(item_src, item_dst)


def main(args):
    # name of project (from args)
    newProjName = args.name
    newProjNameLower = args.name.lower()

    # assuming this script lies in yourgamelib/init/, this computes the relative
    # path from new project root to yourgamelib/
    ygRootAbs = os.path.abspath(os.path.join(os.path.dirname(__file__), ".."))
    templateRoot = os.path.join(ygRootAbs, "init", 'template')
    tgtDir = os.path.join(os.getcwd(), newProjName)
    tgtDirSrc = os.path.join(tgtDir, 'src')
    ygRootRel = os.path.relpath(ygRootAbs, start=tgtDir)
    ygRootRel = ygRootRel.replace(os.sep, posixpath.sep)

    # get git information about yourgamelib
    gitRepo = git.Repo(ygRootAbs)
    gitSha = gitRepo.head.object.hexsha
    print(f"yourgamelib commit: {gitSha}")

    # initialize new project
    if not os.path.isdir(tgtDir):
        os.mkdir(tgtDir)
        print(f"{tgtDir} created")
    else:
        print(f"{tgtDir} already present")

    # create empty assets/ directory if not present
    os.makedirs(os.path.join(tgtDir, 'assets'), exist_ok=True)

    # list content of template to copy for new project
    fileItemsToCopy = os.listdir(templateRoot)

    # If new project does not require template: remove src/, assets/
    if (args.bare or args.noTemplate):
        fileItemsToCopy.remove("assets")
        fileItemsToCopy.remove("src")
        # Stock assets are always copied (if no bare project requested):
        # Copy all asset/ files beginning with "yg_"
        if (args.noTemplate):
            stockAssets = [os.path.join("assets", os.path.basename(a)) for a in glob.glob(
                os.path.join(templateRoot, "assets", "yg_*"))]
            fileItemsToCopy.extend(stockAssets)
        # Bare project: copy only the license file
        else:
            licenseFile = [os.path.join("assets", os.path.basename(a)) for a in glob.glob(
                os.path.join(templateRoot, "assets", "yg_LICENSE*"))]
            fileItemsToCopy.extend(licenseFile)

    print(f"copying these items to {tgtDir}:")
    print("\n".join(fileItemsToCopy))
    copy_file_items(fileItemsToCopy, templateRoot, tgtDir)

    # process template with jinja
    ldr = FileSystemLoader(tgtDir)
    jEnv = Environment(loader=ldr)

    # process CMakeLists.txt
    # todo: extend source file pattern
    dstSources = [f for f in os.listdir(tgtDirSrc) if f.endswith(".cpp")]
    templ = jEnv.get_template("CMakeLists.txt")
    templ.stream(
        YOURGAME_GIT_COMMIT_INIT=gitSha,
        YOURGAME_ROOT=ygRootRel,
        YOURGAME_CLONE=("ON" if args.clone else "OFF"),
        YOURGAME_PROJECT_NAME=newProjName,
        YOURGAME_BARE=args.bare,
        YOURGAME_MY_SOURCES=dstSources,
        YOURGAME_EXT_PROJ=args.extProj
    ).dump(os.path.join(tgtDir, "CMakeLists.txt"))

    # process android build.gradle
    templ = jEnv.get_template("build/android/app/build.gradle")
    templ.stream(
        YOURGAME_PROJECT_NAME_LOWER=newProjNameLower
    ).dump(os.path.join(tgtDir, 'build', 'android', 'app', 'build.gradle'))

    # process android Manifest
    templ = jEnv.get_template("build/android/app/src/main/AndroidManifest.xml")
    templ.stream(
        YOURGAME_PROJECT_NAME=newProjName
    ).dump(os.path.join(tgtDir, 'build', 'android', 'app', 'src', 'main', 'AndroidManifest.xml'))

    # process README.md
    templ = jEnv.get_template("README.md")
    templ.stream(
        YOURGAME_PROJECT_NAME=newProjName,
        YOURGAME_GIT_COMMIT_INIT=gitSha
    ).dump(os.path.join(tgtDir, "README.md"))


if __name__ == "__main__":
    # command line args
    aPars = argparse.ArgumentParser(
        description="Initialize a new YourGameLib project from template")
    aPars.add_argument("name", type=str, help='name of the new project')
    aPars.add_argument('--clone', action='store_true',
                       help='init stand-alone project that clones YourGameLib itself')
    aPars.add_argument('--noTemplate', action='store_true',
                       help='do not copy code and assets from template')
    aPars.add_argument('--bare', action='store_true',
                       help='bare project (no toolbox, minimal dependencies)')
    aPars.add_argument("--extProj", nargs="*", type=str, default=[],
                       help='list of required external projects')
    args = aPars.parse_args()

    main(args)
