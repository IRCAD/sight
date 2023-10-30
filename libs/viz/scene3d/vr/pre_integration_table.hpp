/************************************************************************
 *
 * Copyright (C) 2017-2023 IRCAD France
 * Copyright (C) 2017 IHU Strasbourg
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

#pragma once

#include "viz/scene3d/config.hpp"

#include <data/image.hpp>
#include <data/transfer_function.hpp>

#include <glm/glm.hpp>

#include <OGRE/OgreTexture.h>

namespace sight::viz::scene3d::vr
{

/**
 * @brief Class used to compute the table for pre-integrated rendering.
 * @see Engel 2001 : High-Quality Pre-Integrated Volume Rendering Using Hardware-Accelerated Pixel shading
 */
class pre_integration_table
{
public:

    /// Constructor, does nothing.
    VIZ_SCENE3D_API pre_integration_table();

    /// Destructor, does nothing.
    VIZ_SCENE3D_API ~pre_integration_table();

    /// Initializes the texture resource.
    VIZ_SCENE3D_API void create_texture(const std::string& _parent_id);

    /// Destroy the texture
    VIZ_SCENE3D_API void remove_texture();

    /// Sets the table resolution based on the new image and recomputes it.
    VIZ_SCENE3D_API void image_update(
        const data::image::csptr& _img,
        const data::transfer_function::csptr& _tf,
        float _sampling_rate
    );

    /// Computes the table based on the given TF and slice distance.
    VIZ_SCENE3D_API void tf_update(const data::transfer_function::csptr& _tf, float _sample_distance);

    /// Get the texture holding the pre-integration table.
    [[nodiscard]] VIZ_SCENE3D_API inline Ogre::TexturePtr get_texture() const;

    /// Returns the image value interval.
    [[nodiscard]] VIZ_SCENE3D_API inline std::pair<int, int> get_min_max() const;

private:

    /// Defines a texture pixel.
    struct table_pixel
    {
        uint8_t b;
        uint8_t g;
        uint8_t r;
        uint8_t a;
    };

    /// Pixel structure used when computing the table.
    using integral_pixel = glm::vec4;

    /// Array storing table values.
    table_pixel* m_table {nullptr};

    /// Transfer function integral.
    integral_pixel* m_integral_table {nullptr};

    /// Table's GPU texture.
    Ogre::TexturePtr m_table_texture;

    /// image value interval.
    std::pair<int, int> m_value_interval;

    /// texture resolution.
    unsigned m_texture_size {0};
};

//-----------------------------------------------------------------------------

Ogre::TexturePtr pre_integration_table::get_texture() const
{
    return m_table_texture;
}

//-----------------------------------------------------------------------------

std::pair<int, int> pre_integration_table::get_min_max() const
{
    return m_value_interval;
}

//-----------------------------------------------------------------------------

} // namespace sight::viz::scene3d::vr
