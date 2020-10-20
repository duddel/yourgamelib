#include "yourgame/yourgame.h"
#include "yourgame/gl_include.h"

namespace yg = yourgame; // convenience

namespace mygame
{
    void init(const yg::context &ctx)
    {
        glClearColor(0.0f, 1.0f, 0.0f, 1.0f);
    }

    void tick(const yg::context &ctx)
    {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        if (yg::getInputi(yg::InputSource::YOURGAME_KEY_ESCAPE))
        {
            yg::notifyShutdown();
        }
    }

    void shutdown(const yg::context &ctx) {}

} // namespace mygame
