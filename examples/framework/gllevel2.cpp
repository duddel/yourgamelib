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
#include "stb_image.h"
#include "yourgame/yourgame.h"
#include "yourgame/assetfile.h"
#include "gltexture2d.h"
#include "glshader.h"

namespace mygame
{
    GLTexture2D *loadTexture(const char *filename, GLenum unit)
    {
        int width;
        int height;
        int numChannels;
        auto imgData = yourgame::readAssetFile(filename);
        auto img = stbi_load_from_memory(imgData.data(), imgData.size(), &width, &height, &numChannels, 4);

        if (img)
        {
            yourgame::logd("loaded %v: %vx%vx%v", filename, width, height, numChannels);
            GLTexture2D *texture = GLTexture2D::make(0,
                                                     GL_RGBA8,
                                                     width,
                                                     height,
                                                     0,
                                                     GL_RGBA,
                                                     GL_UNSIGNED_BYTE,
                                                     img,
                                                     unit,
                                                     {{GL_TEXTURE_WRAP_S, GL_REPEAT},
                                                      {GL_TEXTURE_WRAP_T, GL_REPEAT},
                                                      {GL_TEXTURE_MIN_FILTER, GL_LINEAR},
                                                      {GL_TEXTURE_MAG_FILTER, GL_LINEAR}},
                                                     false);
            stbi_image_free(img);
            return texture;
        }
        else
        {
            yourgame::logw("image %v failed to load", filename);
            return nullptr;
        }
    }

    GLShader *loadShader(std::vector<std::pair<GLenum, std::string>> shaderFilenames,
                         std::vector<std::pair<GLuint, std::string>> attrLocs,
                         std::vector<std::pair<GLuint, std::string>> fragDataLocs)
    {
        std::vector<std::pair<GLenum, std::string>> shaderCodes;

        for (const auto &shdrFile : shaderFilenames)
        {
            auto shdrCode = yourgame::readAssetFile(shdrFile.second.c_str());
            shaderCodes.push_back(std::make_pair(shdrFile.first, std::string(shdrCode.begin(), shdrCode.end())));
        }

        std::string shaderErrLog;
        GLShader *newShader = GLShader::make(shaderCodes, attrLocs, fragDataLocs, shaderErrLog);

        if (!newShader)
        {
            yourgame::loge("shader failed to make: %v", shaderErrLog);
        }

        return newShader;
    }
} // namespace mygame
