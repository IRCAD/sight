/************************************************************************
 *
 * Copyright (C) 2016-2022 IRCAD France
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

#include "activity/validator/DefaultActivity.hpp"

#include "activity/validator/registry/macros.hpp"

#include <data/Activity.hpp>
#include <data/Composite.hpp>
#include <data/Vector.hpp>

namespace sight::activity::validator
{

fwActivitiesValidatorRegisterMacro(sight::activity::validator::DefaultActivity);

//-----------------------------------------------------------------------------

DefaultActivity::DefaultActivity(activity::IValidator::Key /*unused*/)
{
}

//-----------------------------------------------------------------------------

DefaultActivity::~DefaultActivity()
= default;

//-----------------------------------------------------------------------------

IValidator::ValidationType DefaultActivity::validate(const data::Activity::csptr& activity) const
{
    return IActivityValidator::checkRequirements(activity);
}

//-----------------------------------------------------------------------------

} // namespace sight::activity::validator
