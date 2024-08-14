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
#include <cstdint>
#include <vector>
#include "doctest.h"
#include "yourgame/yourgame.h"

namespace yg = yourgame; // convenience

TEST_CASE("File")
{
    SUBCASE("readProjectFile_zip_vs_dir")
    {
        // read the same file twice, from .zip and from directory

        std::vector<uint8_t> data_zip;
        std::vector<uint8_t> data_dir;

        yg::file::setProjectPath(yg::file::getAssetFilePath("img.zip"));

        CHECK(yg::file::readFile("p//img_32_16_3.png", data_zip) == 0);

        yg::file::setProjectPath(yg::file::getAssetFilePath());

        CHECK(yg::file::readFile("p//img_32_16_3.png", data_dir) == 0);

        // make sure the file content is identical
        CHECK(data_zip == data_dir);
    }
}
