/************************************************************************
 *
 * Copyright (C) 2020-2021 IRCAD France
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

#include "StereoCamera.hpp"

#include <activity/validator/registry/macros.hpp>

#include <data/Camera.hpp>
#include <data/CameraSeries.hpp>
#include <data/Matrix4.hpp>

namespace sight::module::activity::validator
{

namespace CameraSeries
{

fwActivitiesValidatorRegisterMacro(sight::module::activity::validator::CameraSeries::StereoCamera);

//-----------------------------------------------------------------------------

StereoCamera::StereoCamera(sight::activity::IValidator::Key)
{
}

//-----------------------------------------------------------------------------

StereoCamera::~StereoCamera()
{
}

//-----------------------------------------------------------------------------

sight::activity::IValidator::ValidationType StereoCamera::validate(const data::Object::csptr& currentData) const
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
        if(cameraSeries->numCameras() == 2)
        {
            data::Camera::sptr camera0 = cameraSeries->getCamera(0);
            if(!camera0->getIsCalibrated())
            {
                validation.first  = false;
                validation.second = "The first CameraSeries should be calibrated.";
            }

            data::Camera::sptr camera1 = cameraSeries->getCamera(1);
            if(!camera1->getIsCalibrated())
            {
                validation.first  = false;
                validation.second = "The second CameraSeries should be calibrated.";
            }

            data::Matrix4::sptr matrix = cameraSeries->getExtrinsicMatrix(1);
            if(!matrix)
            {
                validation.first  = false;
                validation.second = "The CameraSeries should contain an extrinsic calibration.";
            }
        }
        else
        {
            validation.first  = false;
            validation.second = "The CameraSeries should contain two cameras.";
        }
    }

    return validation;
}

//-----------------------------------------------------------------------------

} // namespace CameraSeries

} // namespace sight::module::activity::validator
