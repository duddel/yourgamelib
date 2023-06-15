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
#ifndef YOURGAME_IMAGE_H
#define YOURGAME_IMAGE_H

#include <cstdint>
#include <array>
#include <vector>

namespace yourgame
{
   namespace util
   {
      class Image
      {
      public:
         enum class Layout
         {
            RGBA
         };

         /* deleting the copy constructor and the copy assignment operator
         prevents copying (and moving) of the object. */
         Image(Image const &) = delete;
         Image &operator=(Image const &) = delete;

         static Image *fromMemoryEncoded(const uint8_t *data,
                                         size_t numBytes,
                                         bool flipVertically = false,
                                         Layout layoutDst = Layout::RGBA);

         static Image *fromMemoryRaw(const uint8_t *data,
                                     int width,
                                     int height,
                                     Layout layout = Layout::RGBA);

         static Image *fromEmpty(int width,
                                 int height,
                                 Layout layout = Layout::RGBA);

         static std::array<int, 3> decodeImageInfo(const uint8_t *data,
                                                   size_t numBytes);

         static int getNumChannelsFromLayout(Layout layout);

         void premultiplyAlpha();

         void updateRaw(const uint8_t *data,
                        int width,
                        int height,
                        Layout layout = Layout::RGBA);

         const uint8_t *getData() const;

         int getWidth() const;

         int getHeight() const;

         Layout getLayout() const;

         int getNumChannels() const;

         size_t getNumDemandedBytes() const;

      private:
         Image() {}
         std::vector<uint8_t> m_data;
         int m_width;
         int m_height;
         Layout m_layout;
      };
   }
} // namespace yourgame

#endif
