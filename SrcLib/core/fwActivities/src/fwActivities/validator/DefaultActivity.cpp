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


