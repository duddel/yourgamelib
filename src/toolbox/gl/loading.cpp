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
#include <string>
#include <vector>
#include <map>
#include <array>
#include <exception>
#include "tiny_obj_loader.h"
#include "nlohmann/json.hpp"
#include "yourgame/yourgame.h"
#include "yourgame/file.h"
#include "yourgame/gl/loading.h"
#include "yourgame/gl/conventions.h"

using json = nlohmann::json;

namespace yourgame
{
    namespace gl
    {
        Texture *loadTexture(const std::string &imgFilename,
                             const std::string &atlasFilename,
                             const yourgame::gl::TextureConfig &cfg)
        {
            Texture *texture = nullptr;

            std::vector<uint8_t> imgData;
            yourgame::file::readFile(imgFilename, imgData);

            yourgame::util::Image *img =
                yourgame::util::Image::fromMemoryEncoded(imgData.data(), imgData.size(), true);

            if (img)
            {
                if (cfg.premultiplyAlpha)
                {
                    img->premultiplyAlpha();
                }

                std::vector<std::pair<GLenum, GLint>> texParamI = {{GL_TEXTURE_MIN_FILTER, cfg.minMagFilter},
                                                                   {GL_TEXTURE_MAG_FILTER, cfg.minMagFilter},
                                                                   {GL_TEXTURE_WRAP_S, cfg.wrapMode},
                                                                   {GL_TEXTURE_WRAP_T, cfg.wrapMode}};
                texParamI.insert(texParamI.end(), cfg.parameteri.begin(), cfg.parameteri.end());

                texture = Texture::make(GL_TEXTURE_2D, cfg.unit, texParamI);
                texture->updateData(GL_TEXTURE_2D,
                                    0,
                                    GL_RGBA8,
                                    img->getWidth(),
                                    img->getHeight(),
                                    0,
                                    GL_RGBA,
                                    GL_UNSIGNED_BYTE,
                                    img->getData(),
                                    cfg.generateMipmap);
                delete img;
            }
            else
            {
                yourgame::log::warn("loadTexture(): failed to load image file %v", imgFilename);
            }

            // process atlas file
            if (texture && atlasFilename != "")
            {
                std::vector<uint8_t> atlasFile;
                json jAtlas;
                bool jParsed = false;

                if (yourgame::file::readFile(atlasFilename, atlasFile))
                {
                    yourgame::log::warn("loadTexture(): failed to load atlas file %v for image file %v", atlasFilename, imgFilename);
                }
                else
                {
                    try
                    {
                        jAtlas = json::parse(atlasFile);
                        jParsed = true;
                    }
                    catch (std::exception &e)
                    {
                        yourgame::log::warn("loadTexture(): failed to parse atlas file %v (json): %v", atlasFilename, e.what());
                    }
                }

                if (jParsed)
                {
                    // FTP "array"
                    if (jAtlas.contains("frames") && jAtlas["frames"].is_array())
                    {
                        try
                        {
                            for (auto &jFrame : jAtlas["frames"])
                            {
                                bool rotated = jFrame.at("rotated").get<bool>();
                                int pixelWidth;
                                int pixelHeight;

                                // FTP reports the original width and height of a frame,
                                // even if rotated. Therefore, width and height need to
                                // be swapped here to represent the actual dimensions
                                // of the frame in the texture image.
                                if (rotated)
                                {
                                    pixelWidth = jFrame.at("frame").at("h").get<int>();
                                    pixelHeight = jFrame.at("frame").at("w").get<int>();
                                }
                                else
                                {
                                    pixelWidth = jFrame.at("frame").at("w").get<int>();
                                    pixelHeight = jFrame.at("frame").at("h").get<int>();
                                }

                                texture->insertCoords(jFrame.at("filename").get<std::string>(),
                                                      jFrame.at("frame").at("x").get<int>(),
                                                      jFrame.at("frame").at("y").get<int>(),
                                                      pixelWidth,
                                                      pixelHeight,
                                                      rotated);
                            }
                        }
                        catch (std::exception &e)
                        {
                            yourgame::log::warn("loadTexture(): failed to read frames from atlas file %v (json, FTP array): %v", atlasFilename, e.what());
                        }
                    }
                    // FTP "hash"
                    else if (jAtlas.contains("frames") && jAtlas["frames"].is_object())
                    {
                        try
                        {
                            for (auto &jFrame : jAtlas["frames"].items())
                            {
                                bool rotated = jFrame.value().at("rotated").get<bool>();
                                int pixelWidth;
                                int pixelHeight;

                                // FTP reports the original width and height of a frame,
                                // even if rotated. Therefore, width and height need to
                                // be swapped here to represent the actual dimensions
                                // of the frame in the texture image.
                                if (rotated)
                                {
                                    pixelWidth = jFrame.value().at("frame").at("h").get<int>();
                                    pixelHeight = jFrame.value().at("frame").at("w").get<int>();
                                }
                                else
                                {
                                    pixelWidth = jFrame.value().at("frame").at("w").get<int>();
                                    pixelHeight = jFrame.value().at("frame").at("h").get<int>();
                                }

                                texture->insertCoords(jFrame.key(),
                                                      jFrame.value().at("frame").at("x").get<int>(),
                                                      jFrame.value().at("frame").at("y").get<int>(),
                                                      pixelWidth,
                                                      pixelHeight,
                                                      rotated);
                            }
                        }
                        catch (std::exception &e)
                        {
                            yourgame::log::warn("loadTexture(): failed to read frames from atlas file %v (json, FTP hash): %v", atlasFilename, e.what());
                        }
                    }
                    // Crunch
                    else if (jAtlas.contains("textures") && jAtlas["textures"].is_array())
                    {
                        try
                        {
                            // crunch atlas files contain information for multiple
                            // image files, but we only want the current texture to load.
                            for (auto &jTex : jAtlas["textures"])
                            {
                                if (yourgame::file::getFileNameWithoutExtension(imgFilename) ==
                                    jTex.at("name").get<std::string>())
                                {
                                    for (auto &jImg : jTex.at("images"))
                                    {
                                        texture->insertCoords(jImg.at("n").get<std::string>(),
                                                              jImg.at("x").get<int>(),
                                                              jImg.at("y").get<int>(),
                                                              jImg.at("w").get<int>(),
                                                              jImg.at("h").get<int>(),
                                                              false);
                                    }
                                    break;
                                }
                            }
                        }
                        catch (std::exception &e)
                        {
                            yourgame::log::warn("loadTexture(): failed to read frames from atlas file %v (json, Crunch): %v", atlasFilename, e.what());
                        }
                    }
                    else
                    {
                        yourgame::log::warn("loadTexture(): unknown atlas file format %v", atlasFilename);
                    }
                }
            }

            return texture;
        }

        Texture *loadCubemap(const std::vector<std::string> &filenames,
                             const yourgame::gl::TextureConfig &cfg)
        {
            std::vector<std::pair<GLenum, GLint>> texParamI = {{GL_TEXTURE_MIN_FILTER, cfg.minMagFilter},
                                                               {GL_TEXTURE_MAG_FILTER, cfg.minMagFilter},
                                                               {GL_TEXTURE_WRAP_S, cfg.wrapMode},
                                                               {GL_TEXTURE_WRAP_T, cfg.wrapMode},
                                                               {GL_TEXTURE_WRAP_R, cfg.wrapMode}};
            texParamI.insert(texParamI.end(), cfg.parameteri.begin(), cfg.parameteri.end());

            Texture *texture = Texture::make(GL_TEXTURE_CUBE_MAP, cfg.unit, texParamI);

            for (int i = 0; i < filenames.size(); i++)
            {
                auto f = filenames[i];
                std::vector<uint8_t> imgData;
                yourgame::file::readFile(f, imgData);

                yourgame::util::Image *img =
                    yourgame::util::Image::fromMemoryEncoded(imgData.data(), imgData.size(), false);

                if (img)
                {
                    if (cfg.premultiplyAlpha)
                    {
                        img->premultiplyAlpha();
                    }

                    texture->updateData(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
                                        0,
                                        GL_RGBA8,
                                        img->getWidth(),
                                        img->getHeight(),
                                        0,
                                        GL_RGBA,
                                        GL_UNSIGNED_BYTE,
                                        img->getData(),
                                        cfg.generateMipmap);
                    delete img;
                }
                else
                {
                    yourgame::log::warn("image %v failed to load", f);
                    delete texture;
                    return nullptr;
                }
            }

            return texture;
        }

        Shader *loadShader(const std::vector<std::pair<GLenum, std::string>> &shaderFilenames,
                           const std::vector<std::pair<GLuint, std::string>> &attrLocs,
                           const std::vector<std::pair<GLuint, std::string>> &fragDataLocs)
        {
            std::vector<std::pair<GLenum, std::string>> shaderCodes;

            for (const auto &shdrFile : shaderFilenames)
            {
                std::vector<uint8_t> shdrCode;
                yourgame::file::readFile(shdrFile.second, shdrCode);
                std::string shdrStr = std::string(shdrCode.begin(), shdrCode.end());
                shaderCodes.push_back(std::make_pair(shdrFile.first, shdrStr));
            }

            return loadShaderFromStrings(shaderCodes, attrLocs, fragDataLocs);
        }

        Shader *loadShaderFromStrings(const std::vector<std::pair<GLenum, std::string>> &shaderCodes,
                                      const std::vector<std::pair<GLuint, std::string>> &attrLocs,
                                      const std::vector<std::pair<GLuint, std::string>> &fragDataLocs)
        {
            std::string shaderErrLog;

            auto shaderCodesWithVersion = shaderCodes;
            for (auto &shaderCode : shaderCodesWithVersion)
            {
                shaderCode.second = std::string(YOURGAME_GLSL_VERSION_STRING) + "\n" + shaderCode.second;
            }

            Shader *newShader = Shader::make(shaderCodesWithVersion, attrLocs, fragDataLocs, shaderErrLog);

            if (!newShader)
            {
                yourgame::log::error("shader failed to make: %v", shaderErrLog);
            }

            return newShader;
        }

        Geometry *loadGeometry(const std::string &objFilename,
                               const std::string &mtlFilename)
        {
            yourgame::log::debug("loading geometry %v...", objFilename);

            std::vector<uint8_t> objData;
            if (yourgame::file::readFile(objFilename, objData))
            {
                yourgame::log::error("loadGeometry(): failed to load obj file %v", objFilename);
                return nullptr;
            }
            std::string objStr(objData.begin(), objData.end());

            std::string mtlStr = "";
            if (!mtlFilename.empty())
            {
                std::vector<uint8_t> mtlData;
                if (yourgame::file::readFile(mtlFilename, mtlData))
                {
                    yourgame::log::error("loadGeometry(): failed to load mtl file %v", mtlFilename);
                    return nullptr;
                }
                mtlStr = std::string(mtlData.begin(), mtlData.end());
            }

            tinyobj::ObjReader objRdr;
            tinyobj::ObjReaderConfig objRdrCfg;
            objRdrCfg.triangulate = true;
            objRdrCfg.vertex_color = true;
            if (!objRdr.ParseFromString(objStr, mtlStr, objRdrCfg))
            {
                yourgame::log::error("tinyobj::ObjReader::ParseFromString failed");
                return nullptr;
            }

            auto shapes = objRdr.GetShapes();
            auto attribs = objRdr.GetAttrib();
            auto materials = objRdr.GetMaterials();

            yourgame::log::debug("%v shape(s)", shapes.size());
            yourgame::log::debug("%v material(s)", materials.size());

            std::vector<GLuint> objIdxData;
            std::vector<GLuint> objLineIdxData;
            std::vector<GLfloat> objPosData;
            std::vector<GLfloat> objNormalData;
            std::vector<GLfloat> objTexCoordData;
            std::vector<GLfloat> objColordData;

            // each mesh index (here: 3 of them form a triangle (face)), references:
            //   (1) vertex position (and color),
            //   (2) normal,
            //   (3) texture coordinates and
            //   (4) meterial index (of the face),
            // that are parsed separately.
            // uniqueIdxMap is used to track unique combinations of (1-4)
            std::map<std::array<int, 4>, int> uniqueIdxMap;
            GLuint uniqueVertCount = 0U;

            // we merge all obj meshes (triangle shapes) into one GLGeometry shape,
            // and all obj line shapes into another GLGeometry shape
            for (auto const &shape : shapes)
            {
                yourgame::log::debug("shape: %v", shape.name);

                GLuint shapeMeshReadIdx = 0U;
                for (auto const &idx : shape.mesh.indices)
                {
                    // assuming all faces are triangles (objRdrCfg.triangulate is set above),
                    // get the material id for that face (-1, if no material is assigned)
                    int materialId = (shape.mesh.material_ids.at(shapeMeshReadIdx / 3U));
                    auto mapRet = uniqueIdxMap.emplace(
                        std::array<int, 4>{idx.vertex_index, idx.normal_index, idx.texcoord_index, materialId}, uniqueVertCount);
                    if (mapRet.second) // new unique vertex
                    {
                        try
                        {
                            objIdxData.push_back(uniqueVertCount);
                            objPosData.push_back((GLfloat)attribs.vertices.at(idx.vertex_index * 3));
                            objPosData.push_back((GLfloat)attribs.vertices.at(idx.vertex_index * 3 + 1));
                            objPosData.push_back((GLfloat)attribs.vertices.at(idx.vertex_index * 3 + 2));

                            // if material available, use diffuse color as vertex color. if not, use the
                            // (always available, but maybe default) parsed vertex color
                            if (materialId > -1 && materialId < materials.size())
                            {
                                objColordData.push_back((GLfloat)materials.at(materialId).diffuse[0]);
                                objColordData.push_back((GLfloat)materials.at(materialId).diffuse[1]);
                                objColordData.push_back((GLfloat)materials.at(materialId).diffuse[2]);
                                // todo: optionally, make geometry buffers for other material components:
                                // ambient, specular, emissive colors, specular exponent ... ?
                            }
                            else
                            {
                                objColordData.push_back((GLfloat)attribs.colors.at(idx.vertex_index * 3));
                                objColordData.push_back((GLfloat)attribs.colors.at(idx.vertex_index * 3 + 1));
                                objColordData.push_back((GLfloat)attribs.colors.at(idx.vertex_index * 3 + 2));
                            }

                            // if normals available, use them
                            if (attribs.normals.size() > 0)
                            {
                                objNormalData.push_back((GLfloat)attribs.normals.at(idx.normal_index * 3));
                                objNormalData.push_back((GLfloat)attribs.normals.at(idx.normal_index * 3 + 1));
                                objNormalData.push_back((GLfloat)attribs.normals.at(idx.normal_index * 3 + 2));
                            }

                            // if texture coordinates available, use them
                            if (attribs.texcoords.size() > 0)
                            {
                                objTexCoordData.push_back((GLfloat)attribs.texcoords.at(idx.texcoord_index * 2));
                                objTexCoordData.push_back((GLfloat)attribs.texcoords.at(idx.texcoord_index * 2 + 1));
                            }
                        }
                        catch (...)
                        {
                            yourgame::log::error("loadGeometry(): %v has faulty obj vertex data", objFilename);
                            return nullptr;
                        }
                        uniqueVertCount++;
                    }
                    else // reuse unique vertex index
                    {
                        objIdxData.push_back((GLuint)(mapRet.first->second));
                    }
                    shapeMeshReadIdx++;
                }

                // lines. todo: work in progress.
                // - new unique vertices (for lines) are created with material = -1.
                //   (lines do not have material)
                // - as the line vertices share buffers with triangle face vertices,
                //   they must also reference texcoords/normals (if triangle faces do),
                //   in order to keep the vertex data "aligned"
                // - assumption: vertices per line: always 2
                for (auto const &idx : shape.lines.indices)
                {
                    int materialId = -1;
                    auto mapRet = uniqueIdxMap.emplace(
                        std::array<int, 4>{idx.vertex_index, idx.normal_index, idx.texcoord_index, materialId}, uniqueVertCount);
                    if (mapRet.second) // new unique vertex
                    {
                        try
                        {
                            objLineIdxData.push_back(uniqueVertCount);
                            objPosData.push_back((GLfloat)attribs.vertices.at(idx.vertex_index * 3));
                            objPosData.push_back((GLfloat)attribs.vertices.at(idx.vertex_index * 3 + 1));
                            objPosData.push_back((GLfloat)attribs.vertices.at(idx.vertex_index * 3 + 2));
                            objColordData.push_back((GLfloat)attribs.colors.at(idx.vertex_index * 3));
                            objColordData.push_back((GLfloat)attribs.colors.at(idx.vertex_index * 3 + 1));
                            objColordData.push_back((GLfloat)attribs.colors.at(idx.vertex_index * 3 + 2));

                            // if normals available, use them
                            if (attribs.normals.size() > 0)
                            {
                                objNormalData.push_back((GLfloat)attribs.normals.at(idx.normal_index * 3));
                                objNormalData.push_back((GLfloat)attribs.normals.at(idx.normal_index * 3 + 1));
                                objNormalData.push_back((GLfloat)attribs.normals.at(idx.normal_index * 3 + 2));
                            }

                            // if texture coordinates available, use them
                            if (attribs.texcoords.size() > 0)
                            {
                                objTexCoordData.push_back((GLfloat)attribs.texcoords.at(idx.texcoord_index * 2));
                                objTexCoordData.push_back((GLfloat)attribs.texcoords.at(idx.texcoord_index * 2 + 1));
                            }
                        }
                        catch (...)
                        {
                            yourgame::log::error("loadGeometry(): %v has faulty line obj vertex data for", objFilename);
                            return nullptr;
                        }
                        uniqueVertCount++;
                    }
                    else // reuse unique vertex index
                    {
                        objLineIdxData.push_back((GLuint)(mapRet.first->second));
                    }
                }
            }

            Geometry *newGeo = Geometry::make();

            auto vertPosSize = objPosData.size() * sizeof(objPosData[0]);
            auto vertNormSize = objNormalData.size() * sizeof(objNormalData[0]);
            auto vertTexcoordsSize = objTexCoordData.size() * sizeof(objTexCoordData[0]);
            auto vertColorSize = objColordData.size() * sizeof(objColordData[0]);
            auto vertIdxSize = objIdxData.size() * sizeof(objIdxData[0]);
            auto vertLineIdxSize = objLineIdxData.size() * sizeof(objLineIdxData[0]);

            // add available buffers to new Geometry and configure
            // the Geometry shape...
            std::vector<Shape::ArrBufferDescr> arDescrs =
                {{attrLocPosition, 3, GL_FLOAT, GL_FALSE, 0, (void *)0, 0},
                 {attrLocColor, 3, GL_FLOAT, GL_FALSE, 0, (void *)0, 0}};

            std::vector<std::string> arBufferNames = {"pos", "color"};

            newGeo->addBuffer("pos", GL_ARRAY_BUFFER, vertPosSize, objPosData.data(), GL_STATIC_DRAW);
            newGeo->addBuffer("color", GL_ARRAY_BUFFER, vertColorSize, objColordData.data(), GL_STATIC_DRAW);
            newGeo->addBuffer("idx", GL_ELEMENT_ARRAY_BUFFER, vertIdxSize, objIdxData.data(), GL_STATIC_DRAW);
            newGeo->addBuffer("idxLines", GL_ELEMENT_ARRAY_BUFFER, vertLineIdxSize, objLineIdxData.data(), GL_STATIC_DRAW);
            if (vertNormSize > 0)
            {
                newGeo->addBuffer("norm", GL_ARRAY_BUFFER, vertNormSize, objNormalData.data(), GL_STATIC_DRAW);
                arDescrs.push_back({attrLocNormal, 3, GL_FLOAT, GL_FALSE, 0, (void *)0, 0});
                arBufferNames.push_back("norm");
            }
            if (vertTexcoordsSize > 0)
            {
                newGeo->addBuffer("texcoords", GL_ARRAY_BUFFER, vertTexcoordsSize, objTexCoordData.data(), GL_STATIC_DRAW);
                arDescrs.push_back({attrLocTexcoords, 2, GL_FLOAT, GL_FALSE, 0, (void *)0, 0});
                arBufferNames.push_back("texcoords");
            }

            // all obj triangle shapes
            newGeo->addShape("main", arDescrs, arBufferNames, {GL_UNSIGNED_INT, GL_TRIANGLES, (GLsizei)objIdxData.size()}, "idx");

            // all obj line shapes
            newGeo->addShape("lines", arDescrs, arBufferNames, {GL_UNSIGNED_INT, GL_LINES, (GLsizei)objLineIdxData.size()}, "idxLines");

            return newGeo;
        }
    }
} // namespace yourgame
