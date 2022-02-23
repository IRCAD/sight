/************************************************************************
 *
 * Copyright (C) 2020-2022 IRCAD France
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

#include "MonoCamera.hpp"

#include <activity/validator/registry/macros.hpp>

#include <data/Camera.hpp>
#include <data/CameraSeries.hpp>

namespace sight::module::activity::validator
{

namespace CameraSeries
{

fwActivitiesValidatorRegisterMacro(sight::module::activity::validator::CameraSeries::MonoCamera);

//-----------------------------------------------------------------------------

MonoCamera::MonoCamera(sight::activity::IValidator::Key)
{
}

//-----------------------------------------------------------------------------

MonoCamera::~MonoCamera()
{
}

//-----------------------------------------------------------------------------

sight::activity::IValidator::ValidationType MonoCamera::validate(const data::Object::csptr& currentData) const
{
    IValidator::ValidationType validation;

    validation.first  = true;
    validation.second = "";

    data::CameraSeries::csptr cameraSeries = data::CameraSeries::dynamicConstCast(currentData);

    if(!cameraSeries)
    {
        validation.first  = false;
        validation.second = "Current data should be a CameraSeries.";
    }
    else
    {
        if(cameraSeries->numCameras() == 1)
        {
            data::Camera::csptr camera = cameraSeries->getCamera(0);
            if(!camera->getIsCalibrated())
            {
                validation.first  = false;
                validation.second = "The CameraSeries should be calibrated.";
            }
        }
        else
        {
            validation.first  = false;
            validation.second = "The CameraSeries should contain one and only one Camera.";
        }
    }

    return validation;
}

//-----------------------------------------------------------------------------

} // namespace CameraSeries

} // namespace sight::module::activity::validator
