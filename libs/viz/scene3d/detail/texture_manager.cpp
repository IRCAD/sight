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

#include "viz/scene3d/detail/texture_manager.hpp"

#include "viz/scene3d/ogre.hpp"
#include "viz/scene3d/utils.hpp"

#include <ui/__/dialog/message.hpp>

#include <OgreHardwarePixelBuffer.h>

// Usual nolint comment does not work for an unknown reason (clang 17)
// cspell:ignore Wunknown
#ifdef __clang_analyzer__
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wunknown-pragmas"
#endif

namespace sight::viz::scene3d::detail
{

//------------------------------------------------------------------------------

template<typename SRC_TYPE, typename DST_TYPE>
void copy_unsigned_image(Ogre::Texture* _texture, const data::image& _image)
{
    // Get the pixel buffer
    Ogre::HardwarePixelBufferSharedPtr pixel_buffer = _texture->getBuffer();

    // Lock the pixel buffer and copy it
    pixel_buffer->lock(Ogre::HardwareBuffer::HBL_DISCARD);
    const Ogre::PixelBox& pixel_box = pixel_buffer->getCurrentLock();

    using signedType = std::make_signed_t<DST_TYPE>;
    auto p_dest = reinterpret_cast<DST_TYPE*>(pixel_box.data);

    const auto low_bound = []
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

    auto src_buffer = static_cast<const SRC_TYPE*>(_image.buffer());

// NOLINTNEXTLINE(clang-diagnostic-unknown-pragmas)
#pragma omp parallel for shared(p_dest, src_buffer)
    for(Ogre::int32 i = 0 ; i < size ; ++i)
    {
        auto shifted_value = static_cast<DST_TYPE>(src_buffer[i] - low_bound);

        p_dest[i] = shifted_value;
    }

    // Unlock the pixel buffer
    pixel_buffer->unlock();
}

// ----------------------------------------------------------------------------

TextureLoader::return_t TextureLoader::load(const sight::data::image& _image, Ogre::Texture* _texture)
{
    const auto num_dim = _image.numDimensions();
    SIGHT_ASSERT("Only handle 2D and 3D textures", num_dim >= 2 && num_dim <= 3);

    const data::image::Size size = _image.size();

    const auto width  = static_cast<uint32_t>(size[0]);
    const auto height = num_dim >= 2 ? static_cast<uint32_t>(size[1]) : 1;
    const auto depth  = num_dim == 3 ? static_cast<uint32_t>(size[2]) : 1;

    const Ogre::PixelFormat pixel_format = viz::scene3d::utils::getPixelFormatOgre(_image);

    const auto tex_type = num_dim == 2 ? Ogre::TEX_TYPE_2D : Ogre::TEX_TYPE_3D;

    if(_texture->getWidth() != width
       || _texture->getHeight() != height
       || _texture->getDepth() != depth
       || _texture->getTextureType() != tex_type
       || _texture->getFormat() != pixel_format)
    {
        viz::scene3d::utils::allocateTexture(
            _texture,
            size[0],
            size[1],
            depth,
            pixel_format,
            tex_type,
            true
        );
    }

    const auto dump_lock = _image.dump_lock();

    // Workaround because of a bug in Ogre with SNORM formats
    // All SNORM formats are bound to unsigned integers instead of signed integers
    // Thus, we translate the values from [MIN;MAX] to [MIN+(MAX-MIN)/2;MAX+(MAX-MIN)/2]
    const auto src_type = _image.getType();

    if(src_type == core::type::INT8)
    {
        copy_unsigned_image<std::int8_t, std::uint8_t>(_texture, _image);
    }
    else if(src_type == core::type::INT16)
    {
        copy_unsigned_image<std::int16_t, std::uint16_t>(_texture, _image);
    }
    // 32 bits are not well handled in our TF approach. However, most 32bits images fits in 16 bits.
    // So for now, we cast them and assert if the values do not fit.
    else if(src_type == core::type::INT32)
    {
        copy_unsigned_image<std::int32_t, std::uint16_t>(_texture, _image);
    }
    else if(src_type == core::type::UINT32)
    {
        copy_unsigned_image<std::uint32_t, std::uint16_t>(_texture, _image);
    }
    else
    {
        Ogre::Image ogre_image;
        ogre_image.loadDynamicImage(
            static_cast<uint8_t*>(const_cast<void*>(_image.buffer())), // NOLINT(cppcoreguidelines-pro-type-const-cast)
            width,
            height,
            depth,
            pixel_format
        );

        // Copy image's pixel box into texture buffer
        _texture->getBuffer(0, 0)->blitFromMemory(ogre_image.getPixelBox(0, 0));
    }

    return utils::getTextureWindow(src_type);
}

// ----------------------------------------------------------------------------

} // namespace sight::viz::scene3d::detail

#ifdef __clang_analyzer__
#pragma clang diagnostic pop
#endif
