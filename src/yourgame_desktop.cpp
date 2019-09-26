/*
Copyright (c) 2019 Alexander Scholz

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

#include <algorithm>
#include "whereami.h"
#include "yourgame/yourgame.h"

namespace yourgame
{

namespace
{
yourgame::context _context;
void (*_cbInit)(const yourgame::context &ctx) = NULL;
} // namespace

void init()
{
    // get absolute path to assets/ along the executable
    // and make it available via the context
    int exeBasePathLength;
    int exePathLength = wai_getExecutablePath(NULL, 0, NULL);
    char *path = (char *)malloc(exePathLength + 1);
    wai_getExecutablePath(path, exePathLength, &exeBasePathLength);
    path[exeBasePathLength + 1] = '\0';
    _context.assetPath = path;
    free(path);
    std::replace(_context.assetPath.begin(), _context.assetPath.end(), '\\', '/');
    _context.assetPath += "assets/";

    if (_cbInit != NULL)
    {
        _cbInit(_context);
    }
}

void shutdown()
{
}

void registerCbInit(void (*func)(const yourgame::context &ctx))
{
    _cbInit = func;
}

} // namespace yourgame
