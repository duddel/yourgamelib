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
#include <array>
#include <vector>
#include "stb_image.h"
#include "yourgame/util/image.h"

namespace yourgame
{
   namespace util
   {
      Image *Image::fromMemoryEncoded(const uint8_t *data,
                                      size_t numBytes,
                                      bool flipVertically,
                                      Layout layoutDst)
      {
         stbi_set_flip_vertically_on_load(flipVertically);

         int width;
         int height;
         int numChannels;
         auto img = stbi_load_from_memory(static_cast<const stbi_uc *>(data),
                                          numBytes,
                                          &width,
                                          &height,
                                          &numChannels,
                                          Image::getNumChannelsFromLayout(layoutDst));

         stbi_set_flip_vertically_on_load(false);

         if (img)
         {
            Image *newImage = new Image();

            newImage->m_width = width;
            newImage->m_height = height;
            newImage->m_layout = layoutDst;
            newImage->m_data.assign(img, img + newImage->getNumDemandedBytes());

            stbi_image_free(img);

            return newImage;
         }
         else
         {
            return nullptr;
         }
      }

      Image *Image::fromMemoryRaw(const uint8_t *data,
                                  int width,
                                  int height,
                                  Layout layout)
      {
         if (data == nullptr || width <= 0 || height <= 0)
         {
            return nullptr;
         }

         size_t numBytes = width * height * Image::getNumChannelsFromLayout(layout);

         Image *newImage = new Image();
         newImage->m_width = width;
         newImage->m_height = height;
         newImage->m_layout = layout;
         newImage->m_data.assign(data, data + numBytes);

         return newImage;
      }

      Image *Image::fromEmpty(int width,
                              int height,
                              Layout layout)
      {
         if (width <= 0 || height <= 0)
         {
            return nullptr;
         }

         size_t numValues = width * height * Image::getNumChannelsFromLayout(layout);

         Image *newImage = new Image();
         newImage->m_width = width;
         newImage->m_height = height;
         newImage->m_layout = layout;
         newImage->m_data.resize(numValues);

         return newImage;
      }

      std::array<int, 3> Image::decodeImageInfo(const uint8_t *data,
                                                size_t numBytes)
      {
         int x, y, comp;
         stbi_info_from_memory(static_cast<const stbi_uc *>(data),
                               numBytes,
                               &x,
                               &y,
                               &comp);
         return {x, y, comp};
      }

      int Image::getNumChannelsFromLayout(Layout layout)
      {
         // Only RGBA (4 channels) is supported
         return 4;
      }

      void Image::premultiplyAlpha()
      {
         if (m_layout != Image::Layout::RGBA)
         {
            return;
         }

         unsigned int numPixels = m_width * m_height;

         for (unsigned int i = 0; i < numPixels; i++)
         {
            auto rIdx = i * 4;
            auto gIdx = i * 4 + 1;
            auto bIdx = i * 4 + 2;
            auto alpha = m_data.data()[rIdx + 3];
            m_data.data()[rIdx] = (m_data.data()[rIdx] * alpha) / 255;
            m_data.data()[gIdx] = (m_data.data()[gIdx] * alpha) / 255;
            m_data.data()[bIdx] = (m_data.data()[bIdx] * alpha) / 255;
         }
      }

      void Image::updateRaw(const uint8_t *data,
                            int width,
                            int height,
                            Layout layout)
      {
         if (data == nullptr || width <= 0 || height <= 0)
         {
            return;
         }

         m_width = width;
         m_height = height;
         m_layout = layout;

         size_t numBytes = width * height * Image::getNumChannelsFromLayout(layout);
         m_data.assign(data, data + numBytes);
      }

      const uint8_t *Image::getData() const
      {
         return m_data.data();
      }

      int Image::getWidth() const
      {
         return m_width;
      }

      int Image::getHeight() const
      {
         return m_height;
      }

      Image::Layout Image::getLayout() const
      {
         return m_layout;
      }

      int Image::getNumChannels() const
      {
         return Image::getNumChannelsFromLayout(m_layout);
      }

      size_t Image::getNumDemandedBytes() const
      {
         return m_width * m_height * getNumChannels();
      }
   }
} // namespace yourgame
