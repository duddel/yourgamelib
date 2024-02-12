# Quality Tools

-   Tools for code formatting and static analysis
-   We run the tools via Docker, see [/Dockerfile](../Dockerfile)
-   Assuming the local Docker image is tagged with `yourgame`

## Code Formatting (clang-format)

[yg_run-clang-format.py](yg_run-clang-format.py) runs `clang-format` on all YourGameLib source and header files in-place, using the config file [`.clang-format`](../.clang-format) from repository root by default.

```bash
# Bash
docker run --rm -v $(pwd):/code yourgame /bin/bash -c "cd quality && python yg_run-clang-format.py"

# PowerShell
docker run --rm -v ${PWD}:/code yourgame /bin/bash -c "cd quality && python yg_run-clang-format.py"
```

## Static Analysis (clang-tidy)

First, perform CMake configure on YourGameLib to generate a Compilation Database, later used by `clang-tidy`.

```bash
# Bash
docker run --rm -v $(pwd):/code yourgame /bin/bash -c "cd quality && /bin/bash yg_cmake_lib.bash"

# PowerShell
docker run --rm -v ${PWD}:/code yourgame /bin/bash -c "cd quality && /bin/bash yg_cmake_lib.bash"
```

Then, run `clang-tidy`, utilizing the Compilation Database, on all YourGameLib source and header files, using the config file [`.clang-tidy`](../.clang-tidy) from repository root.

```bash
# Bash
docker run --rm -v $(pwd):/code yourgame /bin/bash -c "cd quality && python ./run-clang-tidy.py -config-file ../.clang-tidy -p _build -source-filter '^((?!thirdparty).)*$' > clang-tidy_log.txt"

# PowerShell
docker run --rm -v ${PWD}:/code yourgame /bin/bash -c "cd quality && python ./run-clang-tidy.py -config-file ../.clang-tidy -p _build -source-filter '^((?!thirdparty).)*$' > clang-tidy_log.txt"
```

Optionally, process `clang-tidy` output.

```bash
# Bash
docker run --rm -v $(pwd):/code yourgame /bin/bash -c "cd quality && python ./yg_parse_clang-tidy.py ./clang-tidy_log.txt > clang-tidy_results.txt"

# PowerShell
docker run --rm -v ${PWD}:/code yourgame /bin/bash -c "cd quality && python ./yg_parse_clang-tidy.py ./clang-tidy_log.txt > clang-tidy_results.txt"
```

Example for applying fixes for a single check, `google-readability-casting`.

```bash
docker run --rm -v ${PWD}:/code yourgame /bin/bash -c "cd quality && python ./run-clang-tidy.py -config-file ../.clang-tidy -checks=-*,google-readability-casting -fix -clang-apply-replacements-binary /usr/bin/clang-apply-replacements-18 -p _build -source-filter '^((?!thirdparty).)*$'"
```

## Third-party components

- [run-clang-tidy.py](run-clang-tidy.py)
  - origin https://github.com/llvm/llvm-project/blob/8dfc023e80c35aded33b3e5e4739d3a487b95a7a/clang-tools-extra/clang-tidy/tool/run-clang-tidy.py
  - license: [LICENSE_llvm.txt](LICENSE_llvm.txt)
