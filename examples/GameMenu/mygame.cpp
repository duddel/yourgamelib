/*
Copyright (c) 2019-2020 Alexander Scholz

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
#include "yourgame/gl_include.h"
#include "yourgame/yourgame.h"
#include "yourgame/toolbox.h"
#include "imgui.h"

namespace mygame
{
    void init(const yourgame::context &ctx)
    {
        glClearColor(0.0f, 0.1f, 0.125f, 1.0f);
    }

    void update(const yourgame::context &ctx)
    {
        ImGui::SetNextWindowPos(ImVec2(0.0f, 0.0f));
        ImGui::SetNextWindowSize(ImVec2(ctx.winWidth, ctx.winHeight));

        ImGui::SetNextWindowContentSize(ImVec2(ctx.winWidth, ctx.winHeight));
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));

        ImGui::Begin("main menu", nullptr, (ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoDecoration));

        // menu items
        ImGui::SetCursorPos(ImVec2(ctx.winWidth * 0.05f, ctx.winHeight * 0.45f));
        if (ImGui::Button("START", ImVec2(200.0f, 30.0f)))
        {
        }
        // ImGui::GetCursorScreenPos() can be used as well...
        ImGui::SetCursorPos(ImVec2(ctx.winWidth * 0.05f, ctx.winHeight * 0.45f + 40.0f));
        if (ImGui::Button("SETTINGS", ImVec2(200.0f, 30.0f)))
        {
        }
        ImGui::SetCursorPos(ImVec2(ctx.winWidth * 0.05f, ctx.winHeight * 0.45f + 40.0f * 2.0f));
        if (ImGui::Button("HELP", ImVec2(200.0f, 30.0f)))
        {
        }
        ImGui::SetCursorPos(ImVec2(ctx.winWidth * 0.05f, ctx.winHeight * 0.45f + 40.0f * 3.0f));
        if (ImGui::Button("QUIT", ImVec2(200.0f, 30.0f)))
        {
            yourgame::notifyShutdown();
        }

        ImGui::End();
        ImGui::PopStyleVar();
    }

    void draw(const yourgame::context &ctx)
    {
        glViewport(0, 0, ctx.winWidth, ctx.winHeight);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    }

    void shutdown(const yourgame::context &ctx)
    {
    }
} // namespace mygame
