/************************************************************************
 *
 * Copyright (C) 2017-2024 IRCAD France
 * Copyright (C) 2017-2020 IHU Strasbourg
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

#include "image_center.hpp"

#include <data/helper/medical_image.hpp>

#include <geometry/data/matrix4.hpp>

#include <service/macros.hpp>

namespace sight::module::filter::image
{

//------------------------------------------------------------------------------

image_center::image_center() :
    filter(m_signals)
{
}

//------------------------------------------------------------------------------

void image_center::configuring()
{
}

//------------------------------------------------------------------------------

void image_center::starting()
{
}

//------------------------------------------------------------------------------

void image_center::updating()
{
    const auto image = m_image.const_lock();

    SIGHT_ASSERT("Missing image '" << IMAGE_IN << "'", image);

    const bool image_validity = data::helper::medical_image::check_image_validity(image.get_shared());

    if(!image_validity)
    {
        SIGHT_WARN("Can not compute center of a invalid image.");
        return;
    }

    auto matrix = m_transform.lock();

    SIGHT_ASSERT("Missing matrix '" << TRANSFORM_INOUT << "'", matrix);

    geometry::data::identity(*matrix);

    const auto& origin      = image->origin();
    const auto& orientation = image->orientation();

    // Get World transform
    const glm::dmat4 world_transform {
        orientation[0], orientation[3], orientation[6], 0,
        orientation[1], orientation[4], orientation[7], 0,
        orientation[2], orientation[5], orientation[8], 0,
        origin[0], origin[1], origin[2], 1
    };

    // Get image center in mm
    const auto& size    = image->size();
    const auto& spacing = image->spacing();

    const glm::dvec4 image_center {
        double(size[0]) * spacing[0] / 2.0,
        double(size[1]) * spacing[1] / 2.0,
        double(size[2]) * spacing[2] / 2.0,
        1
    };

    // Compute world center
    const auto world_center = world_transform * image_center;

    // Update the output matrix
    matrix->set_orientation(orientation);
    matrix->set_position(world_center);

    matrix->signal<data::matrix4::modified_signal_t>(data::matrix4::MODIFIED_SIG)->async_emit();
    this->async_emit(signals::COMPUTED);
}

//------------------------------------------------------------------------------

void image_center::stopping()
{
}

//------------------------------------------------------------------------------

service::connections_t image_center::auto_connections() const
{
    return {{IMAGE_IN, data::image::MODIFIED_SIG, service::slots::UPDATE}};
}

//------------------------------------------------------------------------------

} // namespace sight::module::filter::image
