/************************************************************************
 *
 * Copyright (C) 2016-2021 IRCAD France
 * Copyright (C) 2016-2019 IHU Strasbourg
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

#pragma once

#include "activities/config.hpp"
#include "activities/IActivityValidator.hpp"

#include <core/base.hpp>

namespace sight::activities
{
namespace validator
{

/**
 * @brief Defines a validator which checks that activity requirements are valid.
 *
 * It checks if all the required data are present and if the appConfig parameters are found.
 */
class ACTIVITIES_CLASS_API DefaultActivity : public activities::IActivityValidator
{
public:
    fwCoreClassMacro(DefaultActivity, activities::IActivityValidator,
                     activities::validator::factory::New< DefaultActivity >)

    /// Constructor. Do nothing.
    ACTIVITIES_API DefaultActivity(activities::IValidator::Key key);

    /// Destructor. Do nothing.
    ACTIVITIES_API virtual ~DefaultActivity();

    /// Checks if all the required data are present and if the appConfig parameters are found.
    ACTIVITIES_API virtual IValidator::ValidationType validate(
        const CSPTR(data::ActivitySeries)& activitySeries ) const override;
};

} // namespace validator
} // namespace sight::activities
