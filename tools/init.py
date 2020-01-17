import sys
import os
import posixpath
import argparse
from distutils.dir_util import copy_tree
from distutils.file_util import copy_file
from jinja2 import Environment, FileSystemLoader, Template


def copy_file_items(items, src, dst):
    for item in items:
        item_src = os.path.join(src, item)
        item_dst = os.path.join(dst, item)
        if os.path.isdir(item_src):
            copy_tree(item_src, item_dst)
        else:
            copy_file(item_src, item_dst)


# command line args
aPars = argparse.ArgumentParser(
    description="Initialize a new YourGameLib project from template")
aPars.add_argument("name", type=str, help='name of the new project')
aPars.add_argument('--noSources', action='store_true',
                   help='do not copy source files from template')
args = aPars.parse_args()

# name of project (from args)
newProjName = args.name

# assuming this script lies in yourgamelib/tools, this computes the relative
# path from new project root to yourgamelib/
ygRoot = os.path.normpath(os.path.join(os.path.dirname(__file__), '..'))
tgtDir = os.path.join(os.getcwd(), newProjName)
ygRootRel = os.path.relpath(ygRoot, start=tgtDir)
ygRootRel = ygRootRel.replace(os.sep, posixpath.sep)

# initialize new project
if not os.path.isdir(tgtDir):
    os.mkdir(tgtDir)
    print(f"{tgtDir} created")
else:
    print(f"{tgtDir} already present")

# assets/
if not os.path.isdir(os.path.join(tgtDir, 'assets')):
    os.mkdir(os.path.join(tgtDir, 'assets'))
    print(f"{os.path.join(tgtDir, 'assets')} created")

fileItemsToCopy = os.listdir(os.path.join(ygRoot, 'template'))

if(args.noSources):
    fileItemsToCopy = [i for i in fileItemsToCopy if not i.endswith(".cpp")]
    # todo: more patterns to exclude if --noSource requested?

print(f"copying these items to {tgtDir}:")
print("\n".join(fileItemsToCopy))
copy_file_items(fileItemsToCopy, os.path.join(ygRoot, 'template'), tgtDir)

# process template with jinja
ldr = FileSystemLoader(tgtDir)
jEnv = Environment(loader=ldr)

# process CMakeLists.txt
dstSources = [f for f in os.listdir(tgtDir) if f.endswith(".cpp")]
# todo: extend source file pattern

templ = jEnv.get_template("CMakeLists.txt")
templ.stream(
    YOURGAME_ROOT=ygRootRel,
    YOURGAME_PROJECT_NAME=newProjName,
    YOURGAME_MY_SOURCES=dstSources
).dump(os.path.join(tgtDir, "CMakeLists.txt"))
