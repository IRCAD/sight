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
 * @see Engel 2001 : High-Quality Pre-Integrated Volume Rendering Using Hardware-Accelerated Pixel Shading
 */
class pre_integration_table
{
public:

    /// Constructor, does nothing.
    VIZ_SCENE3D_API pre_integration_table();

    /// Destructor, does nothing.
    VIZ_SCENE3D_API ~pre_integration_table();

    /// Initializes the texture resource.
    VIZ_SCENE3D_API void createTexture(const std::string& _parentId);

    /// Destroy the texture
    VIZ_SCENE3D_API void removeTexture();

    /// Sets the table resolution based on the new image and recomputes it.
    VIZ_SCENE3D_API void imageUpdate(
        const data::image::csptr& _img,
        const data::transfer_function::csptr& _tf,
        float _samplingRate
    );

    /// Computes the table based on the given TF and slice distance.
    VIZ_SCENE3D_API void tfUpdate(const data::transfer_function::csptr& _tf, float _sampleDistance);

    /// Get the texture holding the pre-integration table.
    [[nodiscard]] VIZ_SCENE3D_API inline Ogre::TexturePtr getTexture() const;

    /// Returns the image value interval.
    [[nodiscard]] VIZ_SCENE3D_API inline std::pair<int, int> getMinMax() const;

private:

    /// Defines a texture pixel.
    struct TablePixel
    {
        uint8_t b;
        uint8_t g;
        uint8_t r;
        uint8_t a;
    };

    /// Pixel structure used when computing the table.
    typedef glm::vec4 IntegralPixel;

    /// Array storing table values.
    TablePixel* m_table {nullptr};

    /// Transfer function integral.
    IntegralPixel* m_integralTable {nullptr};

    /// Table's GPU texture.
    Ogre::TexturePtr m_tableTexture;

    /// image value interval.
    std::pair<int, int> m_valueInterval;

    /// Texture resolution.
    unsigned m_textureSize {0};
};

//-----------------------------------------------------------------------------

Ogre::TexturePtr pre_integration_table::getTexture() const
{
    return m_tableTexture;
}

//-----------------------------------------------------------------------------

std::pair<int, int> pre_integration_table::getMinMax() const
{
    return m_valueInterval;
}

//-----------------------------------------------------------------------------

} // namespace sight::viz::scene3d::vr
