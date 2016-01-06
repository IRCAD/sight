#include "calibrationActivity/validator/ToolsCalibration.hpp"

#include <fwActivities/validator/registry/macros.hpp>

#include <fwData/Vector.hpp>

#include <fwMedData/Series.hpp>

#include <arData/CameraSeries.hpp>


namespace calibrationActivity
{
namespace validator
{

fwActivitiesValidatorRegisterMacro(
    ::calibrationActivity::validator::ToolsCalibration, "::calibrationActivity::validator::ToolsCalibration");

//-----------------------------------------------------------------------------

ToolsCalibration::ToolsCalibration(::fwActivities::IValidator::Key key)
{
}

//-----------------------------------------------------------------------------

ToolsCalibration::~ToolsCalibration()
{
}

//-----------------------------------------------------------------------------

::fwActivities::IValidator::ValidationType ToolsCalibration::validate(
    const ::fwActivities::registry::ActivityInfo& activityInfo,
    ::fwData::Vector::sptr currentSelection ) const
{
    IValidator::ValidationType validation;

    validation.first  = true;
    validation.second = "";

    for(::fwData::Vector::value_type elt : *currentSelection)
    {
        ::arData::CameraSeries::sptr cameraSeries = ::arData::CameraSeries::dynamicCast(elt);

        if (cameraSeries)
        {
            if (cameraSeries->getNumberOfCameras() == 2)
            {
                ::arData::Camera::sptr camera1 = cameraSeries->getCamera(0);
                ::arData::Camera::sptr camera2 = cameraSeries->getCamera(1);

                if(!camera1->getIsCalibrated() || !camera2->getIsCalibrated())
                {
                    validation.first   = false;
                    validation.second += "Cameras must be calibrated.";
                }
            }
        }
    }

    return validation;
}

//-----------------------------------------------------------------------------

} // namespace validator
} // namespace calibrationActivity



