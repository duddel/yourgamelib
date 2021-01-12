/*
Copyright (c) 2019-2021 Alexander Scholz

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
#ifdef __EMSCRIPTEN__
#include <emscripten/emscripten.h>
#endif
#include "yourgame/yourgame.h"
#include "yourgame_internal/yourgame_internal_desktop.h"

int main(int argc, char *argv[])
{
    yourgame_internal_desktop::init(argc, argv);

#ifdef __EMSCRIPTEN__
    emscripten_set_main_loop(yourgame_internal_desktop::tick, 0, 1);
#else
    while (true)
    {
        yourgame_internal_desktop::tick();
        if (yourgame_internal_desktop::pendingShutdown())
            break;
    }
#endif

    yourgame_internal_desktop::shutdown();

    return 0;
}
