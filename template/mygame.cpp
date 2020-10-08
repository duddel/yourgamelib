#include "yourgame/yourgame.h"
#include "yourgame/gl_include.h"

namespace mygame
{
    void init(const yourgame::context &ctx)
    {
        glClearColor(0.0f, 1.0f, 0.0f, 1.0f);
    }

    void tick(const yourgame::context &ctx)
    {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        if (yourgame::getInputi(yourgame::InputSource::YOURGAME_KEY_ESCAPE))
        {
            yourgame::notifyShutdown();
        }
    }

    void shutdown(const yourgame::context &ctx) {}

} // namespace mygame
