/*
Copyright (c) 2019-2023 Alexander Scholz

This software is provided 'as-is', without any express or implied
warranty. In no event will the authors be held liable for any damages
arising from the use of this software.

Permission is granted to anyone to use this software for any purpose,
including commercial applications, and to alter it and redistribute it
freely, subject to the following restrictions:

1. The origin of this software must not be misrepresented; you must not
   claim that you wrote the original software. If you use this software
   in a product, an acknowledgment in the product documentation would be
   appreciated but is not required.
2. Altered source versions must be plainly marked as such, and must not be
   misrepresented as being the original software.
3. This notice may not be removed or altered from any source distribution.
*/
#define DOCTEST_CONFIG_IMPLEMENT
#include "doctest.h"
#include "yourgame/yourgame.h"

namespace mygame
{
    doctest::Context context;

    void init(int argc, char *argv[])
    {
        context.applyCommandLine(argc, argv);
    }

    void tick()
    {
        yourgame::control::exit();
    }

    int shutdown()
    {
        int ret = context.run();

        // If the test executable should exit, no framework code should be called after
        // context.run(), so exit the application in this case.
        // But currently, there is nothing to be done after context.run() anyway.
        // if(context.shouldExit())
        // {
        //     return ret;
        // }

        return ret;
    }
} // namespace mygame
