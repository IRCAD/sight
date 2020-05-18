/************************************************************************
 *
 * Copyright (C) 2016 IRCAD France
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

#include "validators/CameraSeries/MonoCamera.hpp"

#include <arData/Camera.hpp>
#include <arData/CameraSeries.hpp>

#include <fwActivities/validator/registry/macros.hpp>

namespace validators
{
namespace CameraSeries
{

fwActivitiesValidatorRegisterMacro(::validators::CameraSeries::MonoCamera, "::validators::CameraSeries::MonoCamera");

//-----------------------------------------------------------------------------

MonoCamera::MonoCamera(::fwActivities::IValidator::Key key)
{

}

//-----------------------------------------------------------------------------

MonoCamera::~MonoCamera()
{

}

//-----------------------------------------------------------------------------


::fwActivities::IValidator::ValidationType MonoCamera::validate(const ::fwData::Object::csptr &currentData ) const
{
    IValidator::ValidationType validation;

    validation.first  = true;
    validation.second = "";

    ::arData::CameraSeries::csptr cameraSeries = ::arData::CameraSeries::dynamicConstCast(currentData);

    if (!cameraSeries)
    {
        validation.first  = false;
        validation.second = "Current data should be a CameraSeries.";
    }
    else
    {
        if (cameraSeries->getNumberOfCameras() == 1)
        {
            ::arData::Camera::sptr camera = cameraSeries->getCamera(0);
            if (!camera->getIsCalibrated())
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
} // namespace validators
