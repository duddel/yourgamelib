/*
Copyright (c) 2019-2024 Alexander Scholz

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
#ifndef YOURGAME_YOURGAME_H
#define YOURGAME_YOURGAME_H

// include the actual gl api for the target platform
#include "yourgame/gl_include.h"

// the following includes are part of the yourgame (base) API
#include "yourgame/audio.h"
#include "yourgame/control.h"
#include "yourgame/file.h"
#include "yourgame/input.h"
#include "yourgame/log.h"
#include "yourgame/math.h"
#include "yourgame/time.h"
#include "yourgame/version.h"

// provide the toolbox is requested (YOURGAME_TOOLBOX is defined via Toolbox.cmake)
#ifdef YOURGAME_TOOLBOX
#include "yourgame/toolbox.h"
#endif

#endif
