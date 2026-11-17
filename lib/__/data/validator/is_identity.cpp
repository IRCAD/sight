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

#include "is_identity.hpp"

#include <data/matrix4.hpp>
#include <data/validator/registry/macros.hpp>

namespace sight::data::validator
{

SIGHT_REGISTER_DATA_VALIDATOR(sight::data::validator::is_identity);

//-----------------------------------------------------------------------------

sight::data::validator::return_t is_identity::validate(const data::object::csptr& _object) const
{
    sight::data::validator::return_t validation {true, {}};

    if(const auto matrix = std::dynamic_pointer_cast<const data::matrix4>(_object); matrix)
    {
        if(!matrix->is_identity())
        {
            validation = {false, "Matrix is not the identity matrix."};
        }
    }
    else
    {
        validation = {false, "Current object should be a data::matrix4."};
    }

    return validation;
}

//-----------------------------------------------------------------------------

} // namespace sight::data::validator
