#include "yourgame/yourgame.h"
#include "yourgame/gl_include.h"

namespace yg = yourgame; // convenience

namespace mygame
{
    void init(int argc, char *argv[])
    {
        glClearColor(0.0f, 1.0f, 0.0f, 1.0f);
    }

    void tick()
    {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        if (yg::input::get(yg::input::KEY_ESCAPE))
        {
            yg::control::exit();
        }
    }

    void shutdown()
    {
    }
} // namespace mygame
