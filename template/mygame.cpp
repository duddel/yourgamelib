#include "yourgame/yourgame.h"
#include "yourgame/gl_include.h"

namespace mygame
{

void init(const yourgame::context &ctx)
{
    yourgame::logi("");
    glClearColor(0.0f, 1.0f, 0.0f, 1.0f);
}

void update(const yourgame::context &ctx)
{
    if (yourgame::getInputi(yourgame::InputSource::YOURGAME_KEY_ESCAPE))
    {
        yourgame::notifyShutdown();
    }
}

void draw(const yourgame::context &ctx)
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void shutdown(const yourgame::context &ctx)
{
}

} // namespace mygame
