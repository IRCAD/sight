/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "fwActivities/validator/DefaultActivity.hpp"
#include "fwActivities/validator/registry/macros.hpp"

#include <fwData/Composite.hpp>
#include <fwData/Vector.hpp>

#include <fwMedData/ActivitySeries.hpp>


namespace fwActivities
{
namespace validator
{

fwActivitiesValidatorRegisterMacro(
    ::fwActivities::validator::DefaultActivity, "::fwActivities::validator::DefaultActivity");

//-----------------------------------------------------------------------------

DefaultActivity::DefaultActivity(::fwActivities::IValidator::Key key)
{
}

//-----------------------------------------------------------------------------

DefaultActivity::~DefaultActivity()
{
}

//-----------------------------------------------------------------------------

IValidator::ValidationType DefaultActivity::validate(const ::fwMedData::ActivitySeries::csptr &activitySeries ) const
{
    IValidator::ValidationType validation;

    validation = IActivityValidator::checkRequirements(activitySeries);

    if (validation.first)
    {
        validation = IActivityValidator::checkParameters(activitySeries);
    }

    return validation;
}

//-----------------------------------------------------------------------------

} // namespace validator
} // namespace fwActivities


