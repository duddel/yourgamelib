import os
import glob
import subprocess


WORK_ROOT = os.path.dirname(__file__)
SRC_DIR = os.path.abspath(os.path.join(WORK_ROOT, "../src"))
INC_DIR = os.path.abspath(os.path.join(WORK_ROOT, "../include"))
TEST_DIR = os.path.abspath(os.path.join(WORK_ROOT, "../test"))

TMP_FILE = (os.path.join(WORK_ROOT, "_yourgame_files.txt"))


def main():
    # Get all src and include files
    files = glob.glob(os.path.join(SRC_DIR, "**/*.*"), recursive=True)
    files.extend(glob.glob(os.path.join(INC_DIR, "**/*.*"), recursive=True))
    files.extend(glob.glob(os.path.join(TEST_DIR, "main.cpp"), recursive=True))
    files.extend(glob.glob(os.path.join(TEST_DIR, "testcases/*.*"), recursive=True))

    # Write all files to TMP_FILE
    with open(TMP_FILE, "w") as tmpf:
        for f in files:
            print(f, file=tmpf)

    # Call clang-format in-place
    cmd = ["clang-format", "-i", f"--files={TMP_FILE}"]
    subprocess.call(cmd)

    # Delete TMP_FILE
    os.remove(TMP_FILE)


if __name__ == "__main__":
    main()
