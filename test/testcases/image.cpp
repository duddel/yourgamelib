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

TEST_CASE("Image")
{
    const uint8_t imageDataRaw[] = {0xFF, 0x00, 0x00, 0xFF,
                                    0x00, 0xFF, 0x00, 0xFF,
                                    0x00, 0x00, 0xFF, 0xFF,
                                    0xFF, 0xFF, 0x00, 0xFF};

    const uint8_t imageDataAlpha[] = {255, 0, 0, 0,
                                      1, 123, 0, 255,
                                      0, 0, 127, 127,
                                      66, 99, 193, 55};

    SUBCASE("fromMemoryEncoded_4channels")
    {
        std::vector<uint8_t> imageData;
        CHECK(yg::file::readFile("a//img_32_16_4.png", imageData) == 0);

        yg::util::Image *img = yg::util::Image::fromMemoryEncoded(imageData.data(), imageData.size(), true);
        CHECK(img != nullptr);

        CHECK(img->getWidth() == 32);
        CHECK(img->getHeight() == 16);
        CHECK(img->getLayout() == yg::util::Image::Layout::RGBA);
        CHECK(img->getNumChannels() == 4);

        // image is loaded with flipVertically = true: first pixel
        // is bottom left, which is blue (0,0,255) with opaque alpha (255)
        CHECK(img->getData()[0] == 0);
        CHECK(img->getData()[1] == 0);
        CHECK(img->getData()[2] == 255);
        CHECK(img->getData()[3] == 255);
    }

    SUBCASE("fromMemoryEncoded_3channels")
    {
        std::vector<uint8_t> imageData;
        CHECK(yg::file::readFile("a//img_32_16_3.png", imageData) == 0);

        yg::util::Image *img = yg::util::Image::fromMemoryEncoded(imageData.data(), imageData.size(), true);
        CHECK(img != nullptr);

        CHECK(img->getWidth() == 32);
        CHECK(img->getHeight() == 16);
        CHECK(img->getLayout() == yg::util::Image::Layout::RGBA);
        CHECK(img->getNumChannels() == 4); // resulting Image has 4 channels (RGBA) regardless of file

        // image is loaded with flipVertically = true: first pixel
        // is bottom left, which is blue (0,0,255) with opaque alpha (255)
        CHECK(img->getData()[0] == 0);
        CHECK(img->getData()[1] == 0);
        CHECK(img->getData()[2] == 255);
        CHECK(img->getData()[3] == 255);
    }

    SUBCASE("fromMemoryRaw")
    {
        yg::util::Image *img = nullptr;

        CHECK(yg::util::Image::fromMemoryRaw(nullptr, 2, 2) == nullptr);
        CHECK(yg::util::Image::fromMemoryRaw(imageDataRaw, 0, 2) == nullptr);
        CHECK(yg::util::Image::fromMemoryRaw(imageDataRaw, -1, 2) == nullptr);
        CHECK(yg::util::Image::fromMemoryRaw(imageDataRaw, 2, 0) == nullptr);
        CHECK(yg::util::Image::fromMemoryRaw(imageDataRaw, 2, -1) == nullptr);

        img = yg::util::Image::fromMemoryRaw(imageDataRaw, 2, 2);

        CHECK(img != nullptr);
        CHECK(img->getWidth() == 2);
        CHECK(img->getHeight() == 2);
        CHECK(img->getLayout() == yg::util::Image::Layout::RGBA);
        CHECK(img->getNumChannels() == 4);
        CHECK(img->getData()[4] == 0);
        CHECK(img->getData()[5] == 255);
        CHECK(img->getData()[6] == 0);
        CHECK(img->getData()[7] == 255);
    }

    SUBCASE("fromEmpty")
    {
        CHECK(yg::util::Image::fromEmpty(0, 2) == nullptr);
        CHECK(yg::util::Image::fromEmpty(-1, 2) == nullptr);
        CHECK(yg::util::Image::fromEmpty(2, 0) == nullptr);
        CHECK(yg::util::Image::fromEmpty(2, -1) == nullptr);

        yg::util::Image *img = yg::util::Image::fromEmpty(2, 2);

        CHECK(img != nullptr);
        CHECK(img->getWidth() == 2);
        CHECK(img->getHeight() == 2);
        CHECK(img->getLayout() == yg::util::Image::Layout::RGBA);
        CHECK(img->getNumChannels() == 4);
    }

    SUBCASE("premultiplyAlpha")
    {
        yg::util::Image *img = yg::util::Image::fromMemoryRaw(imageDataAlpha, 2, 2);
        CHECK(img != nullptr);

        CHECK(img->getData()[0] == 255);

        img->premultiplyAlpha();

        // pixel 0: alpha 0, removing all colors
        CHECK(img->getData()[0] == 0);
        CHECK(img->getData()[1] == 0);
        CHECK(img->getData()[2] == 0);
        CHECK(img->getData()[3] == imageDataAlpha[3]);

        // pixel 1: alpha opaque, keep color
        CHECK(img->getData()[4] == imageDataAlpha[4]);
        CHECK(img->getData()[5] == imageDataAlpha[5]);
        CHECK(img->getData()[6] == imageDataAlpha[6]);
        CHECK(img->getData()[7] == imageDataAlpha[7]);

        // pixel 2: alpha 127
        CHECK(img->getData()[8] == 0);
        CHECK(img->getData()[9] == 0);
        CHECK(img->getData()[10] == 63);
        CHECK(img->getData()[11] == imageDataAlpha[11]);

        // pixel 3
        CHECK(img->getData()[12] == imageDataAlpha[12] * imageDataAlpha[15] / 255);
        CHECK(img->getData()[13] == imageDataAlpha[13] * imageDataAlpha[15] / 255);
        CHECK(img->getData()[14] == imageDataAlpha[14] * imageDataAlpha[15] / 255);
        CHECK(img->getData()[15] == imageDataAlpha[15]);
    }

    SUBCASE("updateRaw")
    {
        yg::util::Image *img = yg::util::Image::fromEmpty(4, 8);

        img->updateRaw(imageDataRaw, 2, 2);

        CHECK(img != nullptr);
        CHECK(img->getWidth() == 2);
        CHECK(img->getHeight() == 2);
        CHECK(img->getLayout() == yg::util::Image::Layout::RGBA);
        CHECK(img->getNumChannels() == 4);
        CHECK(img->getData()[4] == 0);
        CHECK(img->getData()[5] == 255);
        CHECK(img->getData()[6] == 0);
        CHECK(img->getData()[7] == 255);
    }

    SUBCASE("decodeImageInfo_4channels")
    {
        std::vector<uint8_t> imageData;
        CHECK(yg::file::readFile("a//img_32_16_4.png", imageData) == 0);

        auto imageInfo = yg::util::Image::decodeImageInfo(imageData.data(), imageData.size());

        CHECK(imageInfo[0] == 32);
        CHECK(imageInfo[1] == 16);
        CHECK(imageInfo[2] == 4);
    }

    SUBCASE("decodeImageInfo_3channels")
    {
        std::vector<uint8_t> imageData;
        CHECK(yg::file::readFile("a//img_32_16_3.png", imageData) == 0);

        auto imageInfo = yg::util::Image::decodeImageInfo(imageData.data(), imageData.size());

        CHECK(imageInfo[0] == 32);
        CHECK(imageInfo[1] == 16);
        CHECK(imageInfo[2] == 3);
    }

    SUBCASE("getNumDemandedBytes")
    {
        yg::util::Image *img = yg::util::Image::fromEmpty(2, 2);
        CHECK(img->getNumDemandedBytes() == 2 * 2 * 4);
    }
}
