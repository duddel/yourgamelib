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
#include <cmath>
#include <vector>
#include "yourgame/assetfile.h"
#include "yourgame/yourgame.h"
#include "yourgame/gl_include.h"
#include "yourgame/input.h"

namespace mygame
{

namespace
{
double _colT = 0.0;
}

void init(const yourgame::context &ctx)
{
    yourgame::logi("mygame::init() called");

    yourgame::logi("attempting to read test.txt from assets:");
    auto testAsset = yourgame::readAssetFile("test.txt");
    yourgame::logi("content: %v", &testAsset[0]);
}

void update(const yourgame::context &ctx)
{
    _colT += (1.0 * ctx.deltaTimeS);

    if(yourgame::getInputi(yourgame::InputSource::YOURGAME_KEY_ESCAPE))
    {
        yourgame::notifyShutdown();
    }

    if (yourgame::getInputi(yourgame::InputSource::YOURGAME_MOUSE_BUTTON_1))
        yourgame::logi("mouse: %v/%v", yourgame::getInputf(yourgame::InputSource::YOURGAME_MOUSE_X),
                       yourgame::getInputf(yourgame::InputSource::YOURGAME_MOUSE_Y));

    if (yourgame::getInputi(yourgame::InputSource::YOURGAME_TOUCH_0_DOWN))
        yourgame::logi("touch 0: %v/%v", yourgame::getInputf(yourgame::InputSource::YOURGAME_TOUCH_0_X),
                       yourgame::getInputf(yourgame::InputSource::YOURGAME_TOUCH_0_Y));
}

void draw(const yourgame::context &ctx)
{
    float colFade = (float)((sin(_colT) * 0.5) + 0.5);

    glClearColor(colFade, 1.0f - colFade, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void shutdown(const yourgame::context &ctx)
{
}

} // namespace mygame
