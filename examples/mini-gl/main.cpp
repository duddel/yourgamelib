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

/*
this is a minimal example setting up gl, involving glfw and glad.
it opens a window with a green-ish clear color.
*/
#include "glad/glad.h"
#define GLFW_INCLUDE_NONE
#include "GLFW/glfw3.h"

int main()
{
    if (!glfwInit())
        return 1;

#if defined(YOURGAME_GL_API_GLES)
    glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_ES_API);
#elif defined(YOURGAME_GL_API_GL)
    glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_API);
#endif
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, YOURGAME_GL_MAJOR);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, YOURGAME_GL_MINOR);
    GLFWwindow *window = glfwCreateWindow(640, 480, "YourGameLib - mini-gl example", NULL, NULL);

    if (window == NULL)
        return 1;

    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);

    gladLoadGL();

    glViewport(0, 0, 640, 480);
    glClearColor(0.0f, 0.7f, 0.0f, 1.0f);

    while (!glfwWindowShouldClose(window))
    {
        glClear(GL_COLOR_BUFFER_BIT);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}
