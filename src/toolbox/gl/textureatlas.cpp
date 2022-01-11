/*
Copyright (c) 2019-2022 Alexander Scholz

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
#include "yourgame/gl/textureatlas.h"

namespace yourgame
{
    namespace gl
    {
        TextureAtlas::~TextureAtlas()
        {
            for (auto t : m_textures)
            {
                delete t;
            }
        }

        void TextureAtlas::pushTexture(yourgame::gl::Texture *newTex)
        {
            m_textures.push_back(newTex);
        }

        void TextureAtlas::pushCoords(std::string name, float uMin, float uMax, float vMin, float vMax)
        {
            m_coords[name] = Coords{uMin, uMax, vMin, vMax, m_textures.back()};

            // matches sprite names that have indexes at the end (with - or _),
            // like walking_01, or walking-01, but NOT: walking01
            static std::regex reSequence("^(.+)[-_](\\d+)$");
            std::smatch reMatch;
            if (std::regex_match(name, reMatch, reSequence) && reMatch.size() == 3)
            {
                // match 0: full string, 1: base, 2: index string
                int seqFrameIdx = std::stoi(reMatch[2].str());
                m_sequences[reMatch[1].str()].seqIdxMap[seqFrameIdx] = name;
            }
        }

        std::pair<yourgame::gl::Texture *, std::array<float, 4>> TextureAtlas::getCoords(std::string name) const
        {
            std::pair<yourgame::gl::Texture *, std::array<float, 4>> ret;

            auto it = m_coords.find(name);
            if (it == m_coords.end())
            {
                ret.first = nullptr;
            }
            else
            {
                ret.first = it->second.tex;
                ret.second[0] = it->second.uMin;
                ret.second[1] = it->second.uMax;
                ret.second[2] = it->second.vMin;
                ret.second[3] = it->second.vMax;
            }

            return ret;
        }

        std::pair<yourgame::gl::Texture *, std::array<float, 4>> TextureAtlas::getCoords(std::string seqName, int seqFrame) const
        {
            auto it = m_sequences.find(seqName);
            if (it == m_sequences.end())
            {
                std::pair<yourgame::gl::Texture *, std::array<float, 4>> ret;
                ret.first = nullptr;
                return ret;
            }
            else
            {
                return getCoords(it->second.seqIdxMap.at(seqFrame % it->second.seqIdxMap.size()));
            }
        }

        int TextureAtlas::getSeqFrames(std::string seqName) const
        {
            auto it = m_sequences.find(seqName);
            return (it == m_sequences.end()) ? 0 : it->second.seqIdxMap.size();
        }

        std::vector<std::string> TextureAtlas::getSequenceNames() const
        {
            std::vector<std::string> seqNames;
            for (const auto &seq : m_sequences)
            {
                seqNames.push_back(seq.first);
            }
            return seqNames;
        }

        yourgame::gl::Texture *TextureAtlas::texture(int idx) const
        {
            try
            {
                return m_textures.at(idx);
            }
            catch (...)
            {
                return nullptr;
            }
        }
    }
} // namespace yourgame
