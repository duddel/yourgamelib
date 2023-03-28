/*
Copyright (c) 2019-2023 Alexander Scholz

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
#ifndef YOURGAME_GLTEXTURE_H
#define YOURGAME_GLTEXTURE_H

#include <map>
#include <string>
#include <vector>
#include "yourgame/gl_include.h"

namespace yourgame
{
    namespace gl
    {
        struct TextureCoords
        {
        public:
            TextureCoords() {}

            TextureCoords(int x, int y, int width, int height, int texWidth, int texHeight, bool cwRot)
            {
                if (width != 0 && height != 0 && texWidth != 0 && texHeight != 0)
                {
                    uMin = static_cast<float>(x) / static_cast<float>(texWidth);
                    uMax = static_cast<float>(x + width) / static_cast<float>(texWidth);
                    vMin = 1.0f - (static_cast<float>(y + height) / static_cast<float>(texHeight));
                    vMax = 1.0f - (static_cast<float>(y) / static_cast<float>(texHeight));
                    xMinPixel = x;
                    xMaxPixel = x + width;
                    yMinPixel = y;
                    yMaxPixel = y + height;
                    aspectRatioPixel = static_cast<float>(width) / static_cast<float>(height);
                    aspectRatioPixelInverse = static_cast<float>(height) / static_cast<float>(width);
                    cwRotated = cwRot;
                }
            }

            float uMin = 0.0f;
            float uMax = 1.0f;
            float vMin = 0.0f;
            float vMax = 1.0f;
            int xMinPixel = 0;
            int xMaxPixel = 1;
            int yMinPixel = 0;
            int yMaxPixel = 1;
            float aspectRatioPixel = 1.0f;
            float aspectRatioPixelInverse = 1.0f;
            bool cwRotated = false;
        };

        class Texture
        {
        public:
            static Texture *make(GLenum target,
                                 GLenum unit,
                                 const std::vector<std::pair<GLenum, GLint>> &parameteri);

            /* deleting the copy constructor and the copy assignment operator
            prevents copying (and moving) of the object. */
            Texture(Texture const &) = delete;
            Texture &operator=(Texture const &) = delete;

            ~Texture();
            void bind() const;
            void unbindTarget() const;

            void updateData(GLenum target,
                            GLint level,
                            GLint internalformat,
                            GLsizei width,
                            GLsizei height,
                            GLint border,
                            GLenum format,
                            GLenum type,
                            const void *data,
                            bool generateMipmap);

            GLsizei getWidth() const { return m_width; }
            GLsizei getHeight() const { return m_height; }
            GLuint handle() const { return m_handle; }

            void insertCoords(std::string name, int x, int y, int width, int height, bool cwRot);

            /**
            \brief returns texture coordinates of subtexture with name name

            \param name name of subtexture to return
             */
            TextureCoords getCoords(std::string name) const;

            /**
            \brief returns texture coordinates of subtexture which is part of a sequence

            \param sequenceName name of the sequence
            \param frame frame index. is always wrapped into valid range. may be negative
             */
            TextureCoords getFrameCoords(std::string sequenceName, int frame);

            /**
            \brief returns the number of frames of a sequence

            \param sequenceName name of the sequence
             */
            int getNumFrames(std::string sequenceName) const;

            /**
            \brief returns a list of the names of all sequences
             */
            std::vector<std::string> getSequenceNames() const;

            /**
            \brief returns texture coordinates of grid subtexture

            \param gridWidth number of grid columns
            \param gridHeight number of grid rows
            \param index index of the grid cell to return coordinates for, starting top-left with index 0
            */
            TextureCoords getGridCoords(int gridWidth, int gridHeight, int index) const;

        private:
            Texture() {}
            GLuint m_handle;
            GLenum m_target;
            GLenum m_unit;
            GLsizei m_width;
            GLsizei m_height;

            struct Sequence
            {
                std::map<int, TextureCoords> frames;
                bool framesInvalidated;
                std::vector<TextureCoords> framesConsec; /**< \brief consecutive representation of the frame coordinates */
            };

            std::map<std::string, TextureCoords> m_coords;
            std::map<std::string, Sequence> m_sequences;
        };
    }
} // namespace yourgame

#endif
