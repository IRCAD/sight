/************************************************************************
 *
 * Copyright (C) 2016-2023 IRCAD France
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

#include <activity/validator/object.hpp>

namespace sight::module::activity::validator::camera_set
{

/**
 * @brief Defines a validator which checks if the current CameraSet contains one and only one Camera and is
 *        calibrated.
 */
class MODULE_ACTIVITY_CLASS_API mono_camera : public sight::activity::validator::object
{
public:

    SIGHT_DECLARE_CLASS(
        mono_camera,
        sight::activity::validator::object,
        sight::activity::validator::factory::make<mono_camera>
    )

    /// Destructor. Do nothing.
    MODULE_ACTIVITY_API ~mono_camera() override = default;

    /**
     * @brief Checks if the current CameraSet contains one and only one Camera and is calibrated.
     * @see module::activity::validator::base::validate
     */
    MODULE_ACTIVITY_API sight::activity::validator::return_t validate(const CSPTR(data::object)& _current_object) const
    override;
};

} // namespace sight::module::activity::validator::camera_set
