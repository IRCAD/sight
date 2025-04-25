/************************************************************************
 *
 * Copyright (C) 2016-2024 IRCAD France
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

#include <sight/activity/config.hpp>

#include "activity/validator/activity.hpp"

#include <core/base.hpp>

namespace sight::activity::validator
{

/**
 * @brief Defines a validator which checks that activity requirements are valid.
 *
 * It checks if all the required data are present and if the appConfig parameters are found.
 */
class SIGHT_ACTIVITY_CLASS_API default_activity : public validator::activity
{
public:

    SIGHT_DECLARE_CLASS(default_activity, validator::activity)

    /// Destructor. Do nothing.
    SIGHT_ACTIVITY_API ~default_activity() override = default;

    /// Checks if all the required data are present and if the appConfig parameters are found.
    SIGHT_ACTIVITY_API validator::return_t validate(
        const CSPTR(data::activity)& _activity
    ) const override;
};

} // namespace sight::activity::validator
