/************************************************************************
 *
 * Copyright (C) 2021-2023 IRCAD France
 * Copyright (C) 2021 IHU Strasbourg
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

#include "geometry/vtk/config.hpp"

#include <data/mesh.hpp>
#include <data/point.hpp>

namespace sight::geometry::vtk
{

/**
 * @brief computeCenterOfMass
 * @param[in] mesh input mesh
 * @param[in] useScalarAsWeights use the scalar data as weights
 * @return mesh center of mass
 */
GEOMETRY_VTK_API data::point::sptr compute_center_of_mass(
    const data::mesh::csptr _mesh,
    bool _use_scalar_as_weights = false
);

} // namespace sight::geometry::vtk
