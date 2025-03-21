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

#include "filled.hpp"

#include <activity/validator/registry/macros.hpp>

#include <data/helper/medical_image.hpp>
#include <data/image.hpp>
#include <data/model_series.hpp>
#include <data/point_list.hpp>

namespace sight::module::activity::validator
{

SIGHT_REGISTER_ACTIVITY_VALIDATOR(sight::module::activity::validator::filled);

//-----------------------------------------------------------------------------

sight::activity::validator::return_t filled::validate(const data::object::csptr& _object) const
{
    sight::activity::validator::return_t validation {true, {}};

    if(const auto image = std::dynamic_pointer_cast<const data::image>(_object); image)
    {
        if(not data::helper::medical_image::check_image_validity(image))
        {
            validation = {false, "Image is empty."};
        }
    }
    else if(const auto model_series = std::dynamic_pointer_cast<const data::model_series>(_object); model_series)
    {
        if(model_series->get_reconstruction_db().empty())
        {
            validation = {false, "Model series does not contain any reconstruction."};
        }
    }
    else if(const auto point_list = std::dynamic_pointer_cast<const data::point_list>(_object); point_list)
    {
        if(point_list->get_points().empty())
        {
            validation = {false, "Point list is empty."};
        }
    }
    else
    {
        validation = {false, "Current object should be an image or an image_series."};
    }

    return validation;
}

//-----------------------------------------------------------------------------

} // namespace sight::module::activity::validator
