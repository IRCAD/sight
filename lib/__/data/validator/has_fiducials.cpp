/************************************************************************
 *
 * Copyright (C) 2025 IRCAD France
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

#include "has_fiducials.hpp"

#include <data/image_series.hpp>
#include <data/validator/registry/macros.hpp>

namespace sight::data::validator
{

SIGHT_REGISTER_DATA_VALIDATOR(sight::data::validator::has_fiducials);

//-----------------------------------------------------------------------------

sight::data::validator::return_t has_fiducials::validate(const data::object::csptr& _object) const
{
    sight::data::validator::return_t validation {true, {}};

    if(const auto image = std::dynamic_pointer_cast<const data::image_series>(_object); image)
    {
        if(auto fiducials_set = image->get_fiducials()->get_fiducial_sets(); fiducials_set.empty())
        {
            validation = {false, "No fiducials in this image."};
        }
        else
        {
            if(std::ranges::all_of(fiducials_set, [](const auto& _set){return _set.fiducial_sequence.empty();}))
            {
                validation = {false, "No fiducials in this image."};
            }
        }
    }
    else
    {
        validation = {false, "Current object should be a DICOM series."};
    }

    return validation;
}

//------------------------------------------------------------------------------

has_fiducials::auto_connect_signals_t has_fiducials::auto_connect_signals() const
{
    return {
        sight::data::object::MODIFIED_SIG,
        sight::data::image_series::signals::POINT_ADDED,
        sight::data::image_series::signals::POINT_INSERTED,
        sight::data::image_series::signals::POINT_REMOVED
    };
}

//-----------------------------------------------------------------------------

} // namespace sight::data::validator
