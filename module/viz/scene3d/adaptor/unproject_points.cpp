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

#include "unproject_points.hpp"

#include <viz/scene3d/helper/camera.hpp>

#include <OgreCamera.h>

namespace sight::module::viz::scene3d::adaptor
{

//------------------------------------------------------------------------------

sight::service::connections_t unproject_points::auto_connections() const
{
    sight::service::connections_t connections;
    connections.push(POINTLIST_IN, sight::data::signals::MODIFIED, sight::service::slots::UPDATE);
    return connections;
}

//-----------------------------------------------------------------------------

void unproject_points::configuring()
{
    this->configure_params();
}

//-----------------------------------------------------------------------------

void unproject_points::starting()
{
    adaptor::init();
    this->updating();
}

//-----------------------------------------------------------------------------

void unproject_points::updating()
{
    using sight::data::point_list;
    using sight::data::point;

    // Get input pointlist.
    const auto point_list_in = m_point_list_in.lock();

    sight::data::point_list::sptr point_list_out = std::make_shared<point_list>();

    // Retrieve camera used to unproject depth
    const Ogre::Camera& camera = *(this->layer()->get_default_camera());

    for(const auto& point : *point_list_in)
    {
        const auto x = static_cast<float>((*point)[0]);
        const auto y = static_cast<float>((*point)[1]);
        const auto z = static_cast<float>((*point)[2]);

        const Ogre::Vector3 pixel_point {x, y, z};
        const Ogre::Vector3 result =
            sight::viz::scene3d::helper::camera::convert_screen_space_to_view_space(camera, pixel_point);

        const point::sptr projected_p = std::make_shared<sight::data::point>(result.x, result.y, result.z);

        point_list_out->push_back(projected_p);
    }

    this->set_output(point_list_out, POINTLIST_OUT);
    point_list_out->async_emit(sight::data::signals::MODIFIED);
}

//-----------------------------------------------------------------------------

void unproject_points::stopping()
{
    this->set_output(nullptr, POINTLIST_OUT);

    adaptor::deinit();
}

//-----------------------------------------------------------------------------

} // namespace sight::module::viz::scene3d::adaptor
