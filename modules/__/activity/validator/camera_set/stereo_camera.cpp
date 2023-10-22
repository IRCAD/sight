/************************************************************************
 *
 * Copyright (C) 2020-2023 IRCAD France
 * Copyright (C) 2016 IHU Strasbourg
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

#include "stereo_camera.hpp"

#include <activity/validator/registry/macros.hpp>

#include <data/camera.hpp>
#include <data/camera_set.hpp>
#include <data/matrix4.hpp>

namespace sight::module::activity::validator::camera_set
{

SIGHT_REGISTER_ACTIVITY_VALIDATOR(sight::module::activity::validator::camera_set::stereo_camera);

//-----------------------------------------------------------------------------

sight::activity::validator::return_t stereo_camera::validate(const data::object::csptr& _current_data) const
{
    sight::activity::validator::return_t validation;

    validation.first  = true;
    validation.second = "";

    const auto& camera_set = std::dynamic_pointer_cast<const data::camera_set>(_current_data);

    if(!camera_set)
    {
        validation.first  = false;
        validation.second = "Current data should be a CameraSet.";
    }
    else
    {
        if(camera_set->size() == 2)
        {
            data::camera::csptr camera0 = camera_set->get_camera(0);
            if(!camera0->getIsCalibrated())
            {
                validation.first  = false;
                validation.second = "The first CameraSet should be calibrated.";
            }

            data::camera::csptr camera1 = camera_set->get_camera(1);
            if(!camera1->getIsCalibrated())
            {
                validation.first  = false;
                validation.second = "The second CameraSet should be calibrated.";
            }

            data::matrix4::csptr matrix = camera_set->get_extrinsic_matrix(1);
            if(!matrix)
            {
                validation.first  = false;
                validation.second = "The CameraSet should contain an extrinsic calibration.";
            }
        }
        else
        {
            validation.first  = false;
            validation.second = "The CameraSet should contain two cameras.";
        }
    }

    return validation;
}

} // namespace sight::module::activity::validator::camera_set
