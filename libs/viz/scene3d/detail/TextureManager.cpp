/************************************************************************
 *
 * Copyright (C) 2022-2023 IRCAD France
 *
 * This file is part of Sight.
 *
 * Sight is free software: you can redistribute it and/or modify it under
 * the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Sight is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with Sight. If not, see <https://www.gnu.org/licenses/>.
 *
 ***********************************************************************/

#include "viz/scene3d/detail/TextureManager.hpp"

#include "viz/scene3d/ogre.hpp"
#include "viz/scene3d/Utils.hpp"

#include <ui/base/dialog/MessageDialog.hpp>

#include <OgreHardwarePixelBuffer.h>

namespace sight::viz::scene3d::detail
{

//------------------------------------------------------------------------------

template<typename SRC_TYPE, typename DST_TYPE>
void copyUnsignedImage(Ogre::Texture* _texture, const data::Image& _image)
{
    // Get the pixel buffer
    Ogre::HardwarePixelBufferSharedPtr pixelBuffer = _texture->getBuffer();

    // Lock the pixel buffer and copy it
    pixelBuffer->lock(Ogre::HardwareBuffer::HBL_DISCARD);
    const Ogre::PixelBox& pixelBox = pixelBuffer->getCurrentLock();

    using signedType = typename std::make_signed<DST_TYPE>::type;
    auto pDest = reinterpret_cast<DST_TYPE*>(pixelBox.data);

    const auto lowBound = []
                          {
                              if constexpr(std::is_signed_v<SRC_TYPE>)
                              {
                                  return std::numeric_limits<signedType>::min();
                              }
                              else
                              {
                                  return static_cast<DST_TYPE>(0);
                              }
                          }();

    const auto size = static_cast<Ogre::int32>(_texture->getWidth() * _texture->getHeight() * _texture->getDepth());

    auto srcBuffer = static_cast<const SRC_TYPE*>(_image.getBuffer());

#pragma omp parallel for shared(pDest, srcBuffer)
    for(Ogre::int32 i = 0 ; i < size ; ++i)
    {
        auto shiftedValue = static_cast<DST_TYPE>(srcBuffer[i] - lowBound);

        pDest[i] = shiftedValue;
    }

    // Unlock the pixel buffer
    pixelBuffer->unlock();
}

// ----------------------------------------------------------------------------

TextureLoader::return_t TextureLoader::load(const sight::data::Image& _image, Ogre::Texture* _texture)
{
    const auto numDim = _image.numDimensions();
    SIGHT_ASSERT("Only handle 2D and 3D textures", numDim >= 2 && numDim <= 3);

    const data::Image::Size size = _image.getSize();

    const auto width  = static_cast<uint32_t>(size[0]);
    const auto height = numDim >= 2 ? static_cast<uint32_t>(size[1]) : 1;
    const auto depth  = numDim == 3 ? static_cast<uint32_t>(size[2]) : 1;

    const Ogre::PixelFormat pixelFormat = viz::scene3d::Utils::getPixelFormatOgre(_image);

    const auto texType = numDim == 2 ? Ogre::TEX_TYPE_2D : Ogre::TEX_TYPE_3D;

    if(_texture->getWidth() != width
       || _texture->getHeight() != height
       || _texture->getDepth() != depth
       || _texture->getTextureType() != texType
       || _texture->getFormat() != pixelFormat)
    {
        viz::scene3d::Utils::allocateTexture(
            _texture,
            size[0],
            size[1],
            depth,
            pixelFormat,
            texType,
            true
        );
    }

    const auto dumpLock = _image.dump_lock();

    // Workaround because of a bug in Ogre with SNORM formats
    // All SNORM formats are bound to unsigned integers instead of signed integers
    // Thus, we translate the values from [MIN;MAX] to [MIN+(MAX-MIN)/2;MAX+(MAX-MIN)/2]
    const auto srcType = _image.getType();

    if(srcType == core::Type::INT8)
    {
        copyUnsignedImage<std::int8_t, std::uint8_t>(_texture, _image);
    }
    else if(srcType == core::Type::INT16)
    {
        copyUnsignedImage<std::int16_t, std::uint16_t>(_texture, _image);
    }
    // 32 bits are not well handled in our TF approach. However, most 32bits images fits in 16 bits.
    // So for now, we cast them and assert if the values do not fit.
    else if(srcType == core::Type::INT32)
    {
        copyUnsignedImage<std::int32_t, std::uint16_t>(_texture, _image);
    }
    else if(srcType == core::Type::UINT32)
    {
        copyUnsignedImage<std::uint32_t, std::uint16_t>(_texture, _image);
    }
    else
    {
        Ogre::Image ogreImage;
        ogreImage.loadDynamicImage(
            static_cast<uint8_t*>(const_cast<void*>(_image.getBuffer())), // NOLINT(cppcoreguidelines-pro-type-const-cast)
            width,
            height,
            depth,
            pixelFormat
        );

        // Copy image's pixel box into texture buffer
        _texture->getBuffer(0, 0)->blitFromMemory(ogreImage.getPixelBox(0, 0));
    }

    return Utils::getTextureWindow(srcType);
}

// ----------------------------------------------------------------------------

} // namespace sight::viz::scene3d::detail
