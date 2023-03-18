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
#include <regex>
#include "yourgame/gl_include.h"
#include "yourgame/gl/texture.h"

namespace yourgame
{
    namespace gl
    {
        Texture *Texture::make(GLenum target,
                               GLenum unit,
                               const std::vector<std::pair<GLenum, GLint>> &parameteri)
        {
            GLuint handle;

            glGenTextures(1, &handle);
            glBindTexture(target, handle);

            for (const auto &par : parameteri)
            {
                glTexParameteri(target, par.first, par.second);
            }

            Texture *newTexture = new Texture();
            newTexture->m_unit = unit;
            newTexture->m_target = target;
            newTexture->m_handle = handle;

            return newTexture;
        }

        Texture::~Texture()
        {
            glDeleteTextures(1, &m_handle);
        }

        void Texture::bind() const
        {
            glActiveTexture(m_unit);
            glBindTexture(m_target, m_handle);
        }

        void Texture::unbindTarget() const
        {
            glActiveTexture(m_unit);
            glBindTexture(m_target, 0);
        }

        void Texture::updateData(GLenum target,
                                 GLint level,
                                 GLint internalformat,
                                 GLsizei width,
                                 GLsizei height,
                                 GLint border,
                                 GLenum format,
                                 GLenum type,
                                 const void *data,
                                 bool generateMipmap)
        {
            glActiveTexture(m_unit);
            glBindTexture(m_target, m_handle);

            glTexImage2D(target, // target can be != m_target (cubemaps...)
                         level,
                         internalformat,
                         width,
                         height,
                         border,
                         format,
                         type,
                         data);

            // todo: does this make sense _here_ for cubemaps?
            if (generateMipmap)
            {
                glGenerateMipmap(m_target);
            }

            glBindTexture(m_target, 0);

            m_width = width;
            m_height = height;
        }

        void Texture::insertCoords(std::string name, int x, int y, int width, int height, bool cwRot)
        {
            if (width == 0 || height == 0 || getWidth() == 0 || getHeight() == 0)
            {
                return;
            }

            m_coords[name] = TextureCoords(x, y, width, height, getWidth(), getHeight(), cwRot);

            // matches sprite names that have indexes at the end (with - or _),
            // like walking_01, or walking-01, but NOT: walking01
            static std::regex reSequence("^(.+)[-_](\\d+)$");
            std::smatch reMatch;
            if (std::regex_match(name, reMatch, reSequence) && reMatch.size() == 3)
            {
                // match 0: full string, 1: base, 2: index string
                int seqFrameIdx = std::stoi(reMatch[2].str());
                m_sequences[reMatch[1].str()].frames[seqFrameIdx] = m_coords[name];

                // make sure framesConsec of this Sequence gets updated
                m_sequences[reMatch[1].str()].framesInvalidated = true;
            }
        }

        TextureCoords Texture::getCoords(std::string name) const
        {
            auto it = m_coords.find(name);
            if (it != m_coords.end())
            {
                return it->second;
            }

            return getGridCoords(1, 1, 0);
        }

        TextureCoords Texture::getFrameCoords(std::string sequenceName, int frame)
        {
            auto it = m_sequences.find(sequenceName);
            if (it != m_sequences.end())
            {
                // update the consecutive representation of the sequence frames
                // if it is invalidated (inserts happened since last getFrameCoords())
                if (it->second.framesInvalidated)
                {
                    it->second.framesConsec.clear();
                    for (const auto &f : it->second.frames)
                    {
                        it->second.framesConsec.push_back(f.second);
                    }
                    it->second.framesInvalidated = false;
                }

                int sizeV = static_cast<int>(it->second.framesConsec.size());
                if (sizeV == 0)
                {
                    return getGridCoords(1, 1, 0);
                }

                // make sure the requested frame index is wrapped into valid range
                int i = (sizeV + (frame % sizeV)) % sizeV;

                return it->second.framesConsec[i];
            }

            return getGridCoords(1, 1, 0);
        }

        int Texture::getNumFrames(std::string sequenceName) const
        {
            auto it = m_sequences.find(sequenceName);
            return (it == m_sequences.end()) ? 0 : it->second.frames.size();
        }

        std::vector<std::string> Texture::getSequenceNames() const
        {
            std::vector<std::string> names;
            for (const auto &seq : m_sequences)
            {
                names.push_back(seq.first);
            }
            return names;
        }

        TextureCoords Texture::getGridCoords(int gridWidth, int gridHeight, int index) const
        {
            if (gridWidth < 1 || gridHeight < 1 || index < 0 || index >= gridWidth * gridHeight)
            {
                return getGridCoords(1, 1, 0);
            }

            float x = (static_cast<float>(index % gridWidth) / gridWidth) * getWidth();
            float y = ((index / gridWidth) / static_cast<float>(gridHeight)) * getHeight();
            int width = getWidth() / gridWidth;
            int height = getHeight() / gridHeight;

            auto coords = TextureCoords(
                static_cast<int>(x),
                static_cast<int>(y),
                width,
                height,
                getWidth(),
                getHeight(),
                false);

            return coords;
        }
    }
} // namespace yourgame
