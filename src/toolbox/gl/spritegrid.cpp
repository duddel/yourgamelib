/*
Copyright (c) 2019-2021 Alexander Scholz

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
#include "yourgame/gl/conventions.h"
#include "yourgame/gl/spritegrid.h"

namespace yourgame
{
    namespace gl
    {
        SpriteGrid::SpriteGrid() {}

        int SpriteGrid::make(const yourgame::gl::TextureAtlas *atlas,
                             const std::vector<std::string> &tiles,
                             unsigned int width,
                             float gridWidth,
                             float gridHeight)
        {
            if (!atlas || tiles.empty())
            {
                return -1;
            }

            // precalc sizes
            unsigned int numTiles = tiles.size();
            auto numVerts = numTiles * 4;
            auto numIdx = numTiles * 6;

            // allocate buffer vectors
            std::vector<GLuint> objIdxData(numIdx, 0U);
            std::vector<GLfloat> objPosData(numVerts * 3);
            std::vector<GLfloat> objTexCoordData(numVerts * 2);
            std::vector<GLfloat> objNormalData(numVerts * 3);
            std::vector<GLfloat> objColordData(numVerts * 3, 1.0f);

            // precalc tile width/height
            auto coords = atlas->getCoords(tiles[0]);
            if (!coords.first)
            {
                return -2;
            }
            float tileAspectNum = (coords.first->width() * (coords.second[1] - coords.second[0]));
            float tileAspectDen = (coords.first->height() * (coords.second[3] - coords.second[2]));
            float tileAspect = (tileAspectNum > 1.0e-6f && tileAspectDen > 1.0e-6f) ? (tileAspectNum / tileAspectDen) : 1.0f;
            float tileWidth = 1.0f;
            float tileHeight = 1.0f;
            // determine the actual number of tiles, the grid is wide:
            auto tilesWide = (width == 0) ? numTiles : (width > numTiles ? numTiles : width);

            if (gridWidth > 0.0f)
            {
                tileWidth = gridWidth / (float)tilesWide;
                if (gridHeight <= 0.0f)
                {
                    tileHeight = tileWidth / tileAspect;
                }
            }

            if (gridHeight > 0.0f)
            {
                auto height = (numTiles / tilesWide) + (numTiles % tilesWide > 0 ? 1 : 0);
                tileHeight = gridHeight / (float)height;
                if (gridWidth <= 0.0f)
                {
                    tileWidth = tileHeight * tileAspect;
                }
            }

            GLfloat orgX = 0.0f;
            GLfloat orgY = 0.0f;
            GLfloat orgZ = 0.0f;

            size_t pWrite = 0;
            size_t tWrite = 0;
            size_t nWrite = 0;
            GLuint vCount = 0U;
            size_t iWrite = 0;
            for (int i = 0; i < numTiles; i++)
            {
                auto coords = atlas->getCoords(tiles[i]);
                if (!coords.first)
                {
                    return -3;
                }

                // positions: 4 vertices, cw, upper left to lower left
                objPosData[pWrite++] = orgX;
                objPosData[pWrite++] = orgY;
                objPosData[pWrite++] = orgZ;
                objPosData[pWrite++] = orgX + (GLfloat)tileWidth;
                objPosData[pWrite++] = orgY;
                objPosData[pWrite++] = orgZ;
                objPosData[pWrite++] = orgX + (GLfloat)tileWidth;
                objPosData[pWrite++] = orgY - (GLfloat)tileHeight;
                objPosData[pWrite++] = orgZ;
                objPosData[pWrite++] = orgX;
                objPosData[pWrite++] = orgY - (GLfloat)tileHeight;
                objPosData[pWrite++] = orgZ;

                // texture coordinates
                objTexCoordData[tWrite++] = (GLfloat)coords.second[0];
                objTexCoordData[tWrite++] = (GLfloat)coords.second[3];
                objTexCoordData[tWrite++] = (GLfloat)coords.second[1];
                objTexCoordData[tWrite++] = (GLfloat)coords.second[3];
                objTexCoordData[tWrite++] = (GLfloat)coords.second[1];
                objTexCoordData[tWrite++] = (GLfloat)coords.second[2];
                objTexCoordData[tWrite++] = (GLfloat)coords.second[0];
                objTexCoordData[tWrite++] = (GLfloat)coords.second[2];

                // normals
                objNormalData[nWrite++] = 0.0f;
                objNormalData[nWrite++] = 0.0f;
                objNormalData[nWrite++] = 1.0f;
                objNormalData[nWrite++] = 0.0f;
                objNormalData[nWrite++] = 0.0f;
                objNormalData[nWrite++] = 1.0f;
                objNormalData[nWrite++] = 0.0f;
                objNormalData[nWrite++] = 0.0f;
                objNormalData[nWrite++] = 1.0f;
                objNormalData[nWrite++] = 0.0f;
                objNormalData[nWrite++] = 0.0f;
                objNormalData[nWrite++] = 1.0f;

                // colors
                // objColordData is filled with 1.0 on construction,
                // leave default color for now

                // vertex indexes
                objIdxData[iWrite++] = (vCount + 0U); // define 2 ccw triangles
                objIdxData[iWrite++] = (vCount + 3U);
                objIdxData[iWrite++] = (vCount + 1U);
                objIdxData[iWrite++] = (vCount + 1U);
                objIdxData[iWrite++] = (vCount + 3U);
                objIdxData[iWrite++] = (vCount + 2U);
                vCount += 4U;

                // set origins for next tile
                if ((i + 1) % tilesWide == 0)
                {
                    orgY -= (GLfloat)(tileHeight);
                    orgX = (GLfloat)0.0f;
                }
                else
                {
                    orgX += (GLfloat)(tileWidth);
                }
            }

            // the data is prepared now...

            auto vertPosSize = objPosData.size() * sizeof(objPosData[0]);
            auto vertNormSize = objNormalData.size() * sizeof(objNormalData[0]);
            auto vertTexcoordsSize = objTexCoordData.size() * sizeof(objTexCoordData[0]);
            auto vertColorSize = objColordData.size() * sizeof(objColordData[0]);
            auto vertIdxSize = objIdxData.size() * sizeof(objIdxData[0]);

            if (!m_geo)
            {
                m_geo = yourgame::gl::Geometry::make();
                // todo: make buffer usage (GL_STATIC_DRAW) configurable
                m_geo->addBuffer("pos", GL_ARRAY_BUFFER, vertPosSize, objPosData.data(), GL_STATIC_DRAW);
                m_geo->addBuffer("norm", GL_ARRAY_BUFFER, vertNormSize, objNormalData.data(), GL_STATIC_DRAW);
                m_geo->addBuffer("texcoords", GL_ARRAY_BUFFER, vertTexcoordsSize, objTexCoordData.data(), GL_STATIC_DRAW);
                m_geo->addBuffer("color", GL_ARRAY_BUFFER, vertColorSize, objColordData.data(), GL_STATIC_DRAW);
                m_geo->addBuffer("idx", GL_ELEMENT_ARRAY_BUFFER, vertIdxSize, objIdxData.data(), GL_STATIC_DRAW);

                std::vector<std::string> arBufferNames = {"pos", "norm", "texcoords", "color"};

                std::vector<yourgame::gl::Shape::ArrBufferDescr> arDescrs =
                    {{gl::attrLocPosition, 3, GL_FLOAT, GL_FALSE, 0, (void *)0, 0},
                     {gl::attrLocNormal, 3, GL_FLOAT, GL_FALSE, 0, (void *)0, 0},
                     {gl::attrLocTexcoords, 2, GL_FLOAT, GL_FALSE, 0, (void *)0, 0},
                     {gl::attrLocColor, 3, GL_FLOAT, GL_FALSE, 0, (void *)0, 0}};

                m_geo->addShape("main", arDescrs, arBufferNames, {GL_UNSIGNED_INT, GL_TRIANGLES, (GLsizei)objIdxData.size()}, "idx");

                // todo: leave m_geo in a valid state if buffer creation failed
            }
            else
            {
                m_geo->bufferData("pos", vertPosSize, objPosData.data());
                m_geo->bufferData("norm", vertNormSize, objNormalData.data());
                m_geo->bufferData("texcoords", vertTexcoordsSize, objTexCoordData.data());
                m_geo->bufferData("color", vertColorSize, objColordData.data());
                m_geo->bufferData("idx", vertIdxSize, objIdxData.data());
                // the shape needs to know how many elements to draw:
                m_geo->setShapeElArDescr("main", {GL_UNSIGNED_INT, GL_TRIANGLES, (GLsizei)objIdxData.size()});

                // todo: leave m_geo in a valid state if buffer creation failed
            }

            return 0;
        }

        SpriteGrid::~SpriteGrid()
        {
            delete m_geo;
        }
    }
} // namespace yourgame
