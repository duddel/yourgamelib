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
#include <string>
#include <vector>
#include <map>
#include <array>
#include "stb_image.h"
#include "tiny_obj_loader.h"
#include "yourgame/yourgame.h"
#include "yourgame/assetfile.h"
#include "gllevel2.h"

namespace mygame
{
    const GLuint attrLocPosition = 0;
    const GLuint attrLocNormal = 1;
    const GLuint attrLocTexcoords = 2;
    const GLuint attrLocColor = 3;
    const std::string attrNamePosition = "inPosition";
    const std::string attrNameNormal = "inNormal";
    const std::string attrNameTexcoords = "inTexcoords";
    const std::string attrNameColor = "inColor";
    const GLchar *unifNameMvpMatrix = "mvpMat";
    const GLchar *unifNameModelMatrix = "modelMat";
    const GLchar *unifNameTexture0 = "texture0";
    const GLchar *unifNameTexture1 = "texture1";

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

    GLGeometry *loadGeometry(const char *objFilename, const char *mtlFilename)
    {
        auto objData = yourgame::readAssetFile(objFilename);
        auto mtlData = yourgame::readAssetFile(mtlFilename);

        std::string objStr(objData.begin(), objData.end());
        std::string mtlStr(mtlData.begin(), mtlData.end());

        tinyobj::ObjReader objRdr;
        tinyobj::ObjReaderConfig objRdrCfg;
        objRdr.ParseFromString(objStr, mtlStr, objRdrCfg);

        auto shapes = objRdr.GetShapes();
        auto attribs = objRdr.GetAttrib();

        std::vector<GLuint> objIdxData;
        std::vector<GLfloat> objPosData;
        std::vector<GLfloat> objNormalData;
        std::vector<GLfloat> objTexCoordData;
        std::vector<GLfloat> objColordData;

        std::map<std::array<int, 3>, int> uniqueIdxMap;
        GLuint uniqueVertCount = 0U;
        // all shapes of obj are merged into one. alternatively, create
        // objIdxData for each shape and add multiple shapes to
        // GLGeometry *newGeo below.
        for (auto const &shape : shapes)
        {
            // move over shape indices and make vertices unique
            // todo: check if assumptions below are valid (number of attrib values per vertex)
            // todo: skip missing attributes (by checking attribs)
            for (auto const &idx : shape.mesh.indices)
            {
                auto mapRet = uniqueIdxMap.emplace(std::array<int, 3>{idx.vertex_index, idx.normal_index, idx.texcoord_index}, uniqueVertCount);
                if (mapRet.second) // new unique vertex
                {
                    objIdxData.push_back(uniqueVertCount);
                    objPosData.push_back((GLfloat)attribs.vertices[idx.vertex_index * 3]);
                    objPosData.push_back((GLfloat)attribs.vertices[idx.vertex_index * 3 + 1]);
                    objPosData.push_back((GLfloat)attribs.vertices[idx.vertex_index * 3 + 2]);
                    objNormalData.push_back((GLfloat)attribs.normals[idx.normal_index * 3]);
                    objNormalData.push_back((GLfloat)attribs.normals[idx.normal_index * 3 + 1]);
                    objNormalData.push_back((GLfloat)attribs.normals[idx.normal_index * 3 + 2]);
                    objTexCoordData.push_back((GLfloat)attribs.texcoords[idx.texcoord_index * 2]);
                    objTexCoordData.push_back((GLfloat)attribs.texcoords[idx.texcoord_index * 2 + 1]);
                    objColordData.push_back((GLfloat)attribs.colors[idx.vertex_index * 3]);
                    objColordData.push_back((GLfloat)attribs.colors[idx.vertex_index * 3 + 1]);
                    objColordData.push_back((GLfloat)attribs.colors[idx.vertex_index * 3 + 2]);

                    uniqueVertCount++;
                }
                else
                {
                    objIdxData.push_back((GLuint)(mapRet.first->second));
                }
            }
        }

        // created Geometry object from parsed obj data
        auto vertPosSize = objPosData.size() * sizeof(objPosData[0]);
        auto vertNormSize = objNormalData.size() * sizeof(objNormalData[0]);
        auto vertTexcoordsSize = objTexCoordData.size() * sizeof(objTexCoordData[0]);
        auto vertIdxSize = objIdxData.size() * sizeof(objIdxData[0]);

        GLGeometry *newGeo = GLGeometry::make();
        newGeo->addBuffer("pos", GL_ARRAY_BUFFER, vertPosSize, objPosData.data(), GL_STATIC_DRAW);
        newGeo->addBuffer("norm", GL_ARRAY_BUFFER, vertNormSize, objNormalData.data(), GL_STATIC_DRAW);
        newGeo->addBuffer("texcoords", GL_ARRAY_BUFFER, vertTexcoordsSize, objTexCoordData.data(), GL_STATIC_DRAW);
        newGeo->addBuffer("idx", GL_ELEMENT_ARRAY_BUFFER, vertIdxSize, objIdxData.data(), GL_STATIC_DRAW);

        newGeo->addShape("main",
                         {{attrLocPosition, 3, GL_FLOAT, GL_FALSE, 0, (void *)0},
                          {attrLocNormal, 3, GL_FLOAT, GL_FALSE, 0, (void *)0},
                          {attrLocTexcoords, 2, GL_FLOAT, GL_FALSE, 0, (void *)0}},
                         {"pos", "norm", "texcoords"},
                         {GL_UNSIGNED_INT, GL_TRIANGLES, (GLsizei)objIdxData.size()},
                         "idx");

        return newGeo;
    }
} // namespace mygame
