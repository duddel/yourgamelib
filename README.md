# YourGameLib

[![Examples](https://github.com/duddel/yourgamelib/workflows/examples/badge.svg)](https://github.com/duddel/yourgamelib/actions?query=workflow%3Aexamples)
[![Docs](https://github.com/duddel/yourgamelib/workflows/docs/badge.svg)](https://github.com/duddel/yourgamelib/actions?query=workflow%3Adocs)
[![Tests](https://github.com/duddel/yourgamelib/workflows/tests/badge.svg)](https://github.com/duddel/yourgamelib/actions?query=workflow%3Atests)

**Framework for game-like applications - based on CMake, C++11, OpenGL (ES)**

-   Kick-start cross-platform OpenGL projects written in C++11
-   Automated integration of common third-party libraries
-   Make use of provided Toolbox and Utilities

## Kick-start

Check out the docs for getting started: <https://duddel.github.io/yourgamelib/>

This is your project:

```cpp
#include "yourgame/yourgame.h"
namespace mygame
{
    void init(int argc, char *argv[]) {
        /* your init code here */
    }

    void tick() {
        /* your game logic here */
        /* your gl code here */
    }

    void shutdown() {
        /* your shutdown code here */
    }
}
```

## License

-   The source code in this repository (except for code in subdirectories of thirdparty/) is licensed under the zlib license (if not stated otherwise)
-   For license information of thirdparty libraries, see: [thirdparty/](thirdparty/)
