/************************************************************************
 *
 * Copyright (C) 2022-2025 IRCAD France
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

#include "viz/scene3d/detail/tf_manager.hpp"

#include "viz/scene3d/ogre.hpp"
#include "viz/scene3d/utils.hpp"

#ifdef WIN32
// OpenGL on windows requires some types defined by the windows API such as WINGDIAPI and APIENTRY.
#include <windows.h>
#endif

#include <GL/gl.h>

#include <OGRE/OgreHardwarePixelBuffer.h>
#include <OGRE/OgreTextureManager.h>

namespace sight::viz::scene3d::detail
{

// ----------------------------------------------------------------------------

tf_loader::return_t tf_loader::load(const sight::data::transfer_function& _tf, Ogre::Texture* _texture)
{
    auto tf_window = Ogre::Vector3(0.0, 0.0, 0.0);

    using value_t   = data::transfer_function::value_t;
    using min_max_t = data::transfer_function::min_max_t;
    std::uint32_t texture_size = ~0U;

    int max_texture_size = 0;
    glGetIntegerv(GL_MAX_TEXTURE_SIZE, &max_texture_size);

    // Retrieves the transfer function's intensity window
    const min_max_t tf_wl_min_max = _tf.window_min_max();

    // The window can be inverted
    const value_t min = std::min(tf_wl_min_max.second, tf_wl_min_max.first);
    const value_t max = std::max(tf_wl_min_max.second, tf_wl_min_max.first);

    const value_t range = std::min(max - min + 1, value_t(_texture->getWidth()));

    if(_tf.resample_to_max_texture_size())
    {
        texture_size = static_cast<std::uint32_t>(max_texture_size);
    }
    else
    {
        texture_size = static_cast<std::uint32_t>(max - min + 1);

        if(texture_size > static_cast<std::uint32_t>(max_texture_size))
        {
            SIGHT_ERROR("Invalid texture size.");
        }
    }

    if(_texture->getTextureType() != Ogre::TEX_TYPE_1D)
    {
        SIGHT_DEBUG("Allocate transfer function: " << _tf.get_id() << " " << texture_size);
        viz::scene3d::utils::allocate_texture(
            _texture,
            texture_size,
            1,
            1,
            Ogre::PF_R8G8B8A8,
            Ogre::TEX_TYPE_2D,
            true
        );
    }

    // Retrieves the pixel buffer from the texture
    Ogre::HardwarePixelBufferSharedPtr pix_buffer = _texture->getBuffer();

    // Discards the entire buffer while locking so that we can easily refill it from scratch
    pix_buffer->lock(Ogre::HardwareBuffer::HBL_DISCARD);
    Ogre::PixelBox pix_box = pix_buffer->getCurrentLock();
    auto* p_dest           = static_cast<std::uint8_t*>(pix_box.data);

    if(_tf.resample_to_max_texture_size())
    {
        // Here we will sample the transfer function in its window
        // We want the minimum and the maximum to be included (so we need +1 extra sample)
        // But we also want one extra point outside each bound window to sample the default color if the tf is clamped,
        // so we need at the end 1+2=3 extra samples
        // We only use the required space of the texture to be more efficient (up to 50x faster)
        const value_t intensity_step = (max - min) / (range - 3);

        value_t i = min - intensity_step;

        // We fill the whole range plus one value for the right bound to avoid rounding errors on the GPU
        // We clamp "manually" in the GLSL
        for(std::uint32_t k = 0 ; k < range ; ++k)
        {
            const auto color = glm::u8vec4(_tf.sample(i) * 255.0);

            *p_dest++ = color.a;
            *p_dest++ = color.b;
            *p_dest++ = color.g;
            *p_dest++ = color.r;

            i += intensity_step;
        }

        tf_window =
            Ogre::Vector3(
                float(min - intensity_step),
                float(max + intensity_step),
                float((max - min) / _texture->getWidth())
            );
    }
    else
    {
        // Here we will sample the transfer function in its window
        // We want the minimum and the maximum to be included (so we need +1 extra sample)
        // But we also want one extra point outside each bound window to sample the default color if the tf is clamped,
        // so we need at the end 1+2=3 extra samples
        // We only use the required space of the texture to be more efficient (up to 50x faster)
        value_t i = min;

        // We fill the whole range plus one value for the right bound to avoid rounding errors on the GPU
        // We clamp "manually" in the GLSL
        for(std::uint32_t k = 0 ; k < range ; ++k)
        {
            const auto color = glm::u8vec4(_tf.sample(i) * 255.0);

            *p_dest++ = color.a;
            *p_dest++ = color.b;
            *p_dest++ = color.g;
            *p_dest++ = color.r;

            i++;
        }

        tf_window =
            Ogre::Vector3(
                float(min),
                float(max),
                float((max - min + 1) / _texture->getWidth())
            );
    }

    pix_buffer->unlock();

    return tf_window;
}

// ----------------------------------------------------------------------------

} // namespace sight::viz::scene3d::detail
