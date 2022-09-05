/************************************************************************
 *
 * Copyright (C) 2016-2022 IRCAD France
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

#include "modules/activity/config.hpp"

#include <activity/IObjectValidator.hpp>

namespace sight::module::activity::validator::CameraSeries
{

/**
 * @brief Defines a validator which checks if the current CameraSeries contains one and only one Camera and is
 *        calibrated.
 */
class MODULE_ACTIVITY_CLASS_API MonoCamera : public sight::activity::IObjectValidator
{
public:

    SIGHT_DECLARE_CLASS(
        MonoCamera,
        sight::activity::IObjectValidator,
        sight::activity::validator::factory::New<MonoCamera>
    )

    /// Constructor. Do nothing.
    MODULE_ACTIVITY_API MonoCamera(sight::activity::IValidator::Key key);

    /// Destructor. Do nothing.
    MODULE_ACTIVITY_API ~MonoCamera() override;

    /**
     * @brief Checks if the current CameraSeries contains one and only one Camera and is calibrated.
     * @see module::activity::IValidator::validate
     */
    MODULE_ACTIVITY_API IValidator::ValidationType validate(const CSPTR(data::Object)& currentObject) const override;
};

} // namespace sight::module::activity::validator::CameraSeries
