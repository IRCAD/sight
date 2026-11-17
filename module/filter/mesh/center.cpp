/************************************************************************
 *
 * Copyright (C) 2025-2026 IRCAD France
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

#include "center.hpp"

#include <geometry/data/matrix4.hpp>

namespace sight::module::filter::mesh
{

//------------------------------------------------------------------------------

center::center() :
    filter(has_signals::signals())
{
}

//------------------------------------------------------------------------------

void center::configuring()
{
}

//------------------------------------------------------------------------------

void center::starting()
{
}

//------------------------------------------------------------------------------

void center::updating()
{
    const auto mesh = m_mesh.lock();
    SIGHT_ASSERT("Missing mesh", mesh);

    auto matrix = m_transform.lock();
    SIGHT_ASSERT("Missing matrix", matrix);

    // Reset to identity
    geometry::data::identity(*matrix);

    const auto bbox = mesh->get_bounding_box();

    // Update the output matrix: keep identity orientation, set translation to center
    matrix->set_position(
        {(bbox.max[0] + bbox.min[0]) * 0.5,
         (bbox.max[1] + bbox.min[1]) * 0.5,
         (bbox.max[2] + bbox.min[2]) * 0.5
        });

    matrix->async_emit(data::signals::MODIFIED);
    this->async_emit(signals::SUCCEEDED);
}

//------------------------------------------------------------------------------

void center::stopping()
{
}

//------------------------------------------------------------------------------

service::connections_t center::auto_connections() const
{
    return {{m_mesh, data::signals::MODIFIED, service::slots::UPDATE}};
}

//------------------------------------------------------------------------------

} // namespace sight::module::filter::mesh
