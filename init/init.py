import os
import posixpath
import argparse
import shutil
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
    print("yourgamelib commit: {}".format(gitSha))

    # initialize new project
    if not os.path.isdir(tgtDir):
        os.mkdir(tgtDir)
        print("{} created".format(tgtDir))
    else:
        print("{} already present".format(tgtDir))

    # list content of template to copy for new project
    fileItemsToCopy = os.listdir(templateRoot)

    # do not copy template code (src/) and assets/ if project is bare or
    # contains code + assets already (no stub)
    if (args.noStub or args.bare):
        fileItemsToCopy.remove("assets")
        fileItemsToCopy.remove("src")

    print("copying these items to {}:".format(tgtDir))
    print("\n".join(fileItemsToCopy))
    copy_file_items(fileItemsToCopy, templateRoot, tgtDir)

    # create empty assets/ directory if not present
    if not os.path.isdir(os.path.join(tgtDir, 'assets')):
        os.mkdir(os.path.join(tgtDir, 'assets'))
        print("{} created".format(os.path.join(tgtDir, 'assets')))

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
    aPars.add_argument('--noStub', action='store_true',
                       help='do not copy source files and assets/ from template')
    aPars.add_argument('--bare', action='store_true',
                       help='bare project (no toolbox, minimal dependencies)')
    aPars.add_argument("--extProj", nargs="*", type=str, default=[],
                       help='list of required external projects')
    args = aPars.parse_args()

    main(args)
