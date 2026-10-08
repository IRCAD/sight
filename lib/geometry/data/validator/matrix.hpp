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

#pragma once

#include <sight/geometry/data/config.hpp>

#include <data/validator/base.hpp>

namespace sight::geometry::data::matrix
{

/**
 * @brief Validates that a matrix4 is a homogeneous orthogonal transformation matrix.
 */
class SIGHT_GEOMETRY_DATA_CLASS_API validator final : public sight::data::validator::base
{
public:

    SIGHT_DECLARE_CLASS(validator, sight::data::validator::base, sight::data::validator::factory::make<validator>);
    SIGHT_GEOMETRY_DATA_API ~validator() final = default;

    SIGHT_GEOMETRY_DATA_API sight::data::validator::return_t validate(
        const sight::data::object::csptr& _object
    ) const final;
};

} // namespace sight::geometry::data::matrix
