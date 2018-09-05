/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "validators/CameraSeries/StereoCamera.hpp"

#include <fwActivities/validator/registry/macros.hpp>

#include <arData/Camera.hpp>
#include <arData/CameraSeries.hpp>

#include <fwData/TransformationMatrix3D.hpp>

namespace validators
{
namespace CameraSeries
{

fwActivitiesValidatorRegisterMacro(::validators::CameraSeries::StereoCamera,
                                   "::validators::CameraSeries::StereoCamera");

//-----------------------------------------------------------------------------

StereoCamera::StereoCamera(::fwActivities::IValidator::Key key)
{

}

//-----------------------------------------------------------------------------

StereoCamera::~StereoCamera()
{

}

//-----------------------------------------------------------------------------


::fwActivities::IValidator::ValidationType StereoCamera::validate(const ::fwData::Object::csptr &currentData ) const
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
        if (cameraSeries->getNumberOfCameras() == 2)
        {
            ::arData::Camera::sptr camera0 = cameraSeries->getCamera(0);
            if (!camera0->getIsCalibrated())
            {
                validation.first  = false;
                validation.second = "The first CameraSeries should be calibrated.";
            }
            ::arData::Camera::sptr camera1 = cameraSeries->getCamera(1);
            if (!camera1->getIsCalibrated())
            {
                validation.first  = false;
                validation.second = "The second CameraSeries should be calibrated.";
            }

            ::fwData::TransformationMatrix3D::sptr matrix = cameraSeries->getExtrinsicMatrix(1);
            if (!matrix)
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
} // namespace validators
