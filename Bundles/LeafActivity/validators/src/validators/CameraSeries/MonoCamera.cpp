/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

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
