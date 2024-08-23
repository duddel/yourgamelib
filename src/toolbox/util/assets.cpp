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
#include "yourgame/util/assetmanager.h"
#include "yourgame/gl/loading.h"

namespace yourgame_internal
{
    namespace util
    {
        namespace assets
        {
            yourgame::util::AssetManager manager;

            bool loadStockAssets()
            {
                bool ret = true;

                // Geometry
                ret = ret && manager.insert("geoQuad", yourgame::gl::loadGeometry("a//yg_quad.obj"));
                ret = ret && manager.insert("geoGrid", yourgame::gl::loadGeometry("a//yg_grid.obj"));

                // Shaders
                ret = ret && manager.insert("shaderPostNull", yourgame::gl::loadShader(
                                                                  {{GL_VERTEX_SHADER, "a//yg_post.vert"},
                                                                   {GL_FRAGMENT_SHADER, "a//yg_post_null.frag"}}));

                ret = ret && manager.insert("shaderSprite", yourgame::gl::loadShader(
                                                                {{GL_VERTEX_SHADER, "a//yg_sprite.vert"},
                                                                 {GL_FRAGMENT_SHADER, "a//yg_sprite.frag"}}));

                if (!ret)
                {
                    manager.clear();
                }

                return ret;
            }

            void clearStockAssets()
            {
                manager.clear();
            }

        } // namespace assets
    } // namespace util
} // namespace yourgame_internal
