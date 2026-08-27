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

#include "matrix.hpp"

#include <data/matrix4.hpp>
#include <data/validator/registry/macros.hpp>

#include <geometry/data/matrix4.hpp>

namespace sight::geometry::data::matrix
{

SIGHT_REGISTER_DATA_VALIDATOR(sight::geometry::data::matrix::validator);

//-----------------------------------------------------------------------------

sight::data::validator::return_t validator::validate(const sight::data::object::csptr& _object) const
{
    const auto matrix = std::dynamic_pointer_cast<const sight::data::matrix4>(_object);

    if(!matrix)
    {
        return {false, "Current object should be a data::matrix4."};
    }

    if(!sight::geometry::data::is_orthogonal(*matrix))
    {
        SIGHT_ERROR("Matrix is not orthogonal.");
        return {false, "Matrix is not orthogonal."};
    }

    if(!sight::geometry::data::is_homogeneous(*matrix))
    {
        SIGHT_ERROR("Matrix is not homogeneous.");
        return {false, "Matrix is not homogeneous."};
    }

    return {true, {}};
}

//-----------------------------------------------------------------------------

} // namespace sight::geometry::data::matrix
