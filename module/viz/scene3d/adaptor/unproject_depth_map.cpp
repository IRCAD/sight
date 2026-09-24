/************************************************************************
 *
 * Copyright (C) 2026 IRCAD France
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

#include "unproject_depth_map.hpp"

#include <OgreCamera.h>

#include <glm/geometric.hpp> // NOLINT(misc-include-cleaner)
#include <glm/vec3.hpp>      // NOLINT(misc-include-cleaner)
#include <viz/scene3d/helper/camera.hpp>

namespace sight::module::viz::scene3d::adaptor
{

//------------------------------------------------------------------------------

sight::service::connections_t unproject_depth_map::auto_connections() const
{
    sight::service::connections_t connections;
    connections.push(DEPTH_MAP_IN, sight::data::signals::MODIFIED, sight::service::slots::UPDATE);
    connections.push(DEPTH_MAP_IN, sight::data::image::signals::BUFFER_MODIFIED, sight::service::slots::UPDATE);
    return connections;
}

//-----------------------------------------------------------------------------

void unproject_depth_map::configuring()
{
    this->configure_params();
}

//-----------------------------------------------------------------------------

void unproject_depth_map::starting()
{
    adaptor::init();
}

//-----------------------------------------------------------------------------

void unproject_depth_map::updating()
{
    // Get depth map
    const auto depth_map_in = m_depth_map_in.lock();

    SIGHT_ASSERT(
        "Input '" << DEPTH_MAP_IN << "' needs to be in two dimension",
        depth_map_in->num_dimensions() == 2
    );
    SIGHT_ASSERT(
        "Input '" << DEPTH_MAP_IN << "' needs to have unsigned char pixels",
        depth_map_in->type() == sight::core::type::UINT8
    );
    SIGHT_ASSERT(
        "Input '" << DEPTH_MAP_IN << "' needs at least three components",
        depth_map_in->num_components() >= 3
    );

    // Get the size
    using storage_type_t = sight::data::image::size_t::value_type;
    const storage_type_t x_max = depth_map_in->size()[0];
    const storage_type_t y_max = depth_map_in->size()[1];

    // Create the output
    auto depth_map_out = std::make_shared<sight::data::image>();
    depth_map_out->resize(
        depth_map_in->size(),
        sight::core::type::FLOAT32,
        sight::data::image::gray_scale
    );

    // Retrieve camera used to unproject depth
    ::Ogre::Camera& camera = *(this->layer()->get_default_camera());

    auto in_it  = depth_map_in->begin<sight::data::iterator::rgba>();
    auto out_it = depth_map_out->begin<float>();

    // Unproject each depth into the view space
    for(size_t x = 0 ; x < x_max ; ++x)
    {
        for(size_t y = 0 ; y < y_max ; ++y)
        {
            const int pos = static_cast<int>(y * x_max + x);
            const auto in = in_it + pos;

            const float max24int = 256.F * 256.F * 256.F - 1.F;

            const float r1 = static_cast<float>(in->r) / 255.F;
            const float g1 = static_cast<float>(in->g) / 255.F;
            const float b1 = static_cast<float>(in->b) / 255.F;

            const float depth_screen_space = 255.F
                                             * glm::dot(glm::vec3(r1, g1, b1), glm::vec3(256.F * 256.F, 256.F, 1.F))
                                             / max24int;

            const ::Ogre::Vector3 pixel_point(static_cast< ::Ogre::Real>(x),
                                              static_cast< ::Ogre::Real>(y),
                                              depth_screen_space);

            const ::Ogre::Vector3 result = sight::viz::scene3d::helper::camera::convert_screen_space_to_view_space(
                camera,
                pixel_point
            );

            const auto out = out_it + pos;

            // The near is considered as zero
            if(std::abs(result.z - static_cast<float>(camera.getNearClipDistance()))
               <= std::numeric_limits<float>::epsilon())
            {
                *out = 0.F;
            }
            else
            {
                *out = result.z;
            }
        }
    }

    // Set the ouput of the service
    this->set_output(depth_map_out, DEPTH_MAP_OUT);
}

//-----------------------------------------------------------------------------

void unproject_depth_map::stopping()
{
    adaptor::deinit();
}

//-----------------------------------------------------------------------------

} // namespace sight::module::viz::scene3d::adaptor
