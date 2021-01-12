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
#include <cstdint>
#include <cstdio>
#include <vector>

namespace yourgame_internal
{
    int readFile(const char *filename, std::vector<uint8_t> &dst)
    {
        std::FILE *f = std::fopen(filename, "rb");
        if (f)
        {
            std::fseek(f, 0, SEEK_END);
            auto nBytes = std::ftell(f);
            dst.resize(nBytes);
            std::rewind(f);
            std::fread(&dst[0], 1, dst.size(), f);
            std::fclose(f);
            return 0;
        }
        return -1;
    }

    int writeFile(const char *filename, const void *data, size_t numBytes)
    {
        int ret = -1;
        std::FILE *f = std::fopen(filename, "wb");
        if (f)
        {
            std::fwrite(data, 1, numBytes, f);
            ret = std::ferror(f);
            std::fclose(f);
        }
        return ret;
    }
} // namespace yourgame_internal
