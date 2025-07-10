/************************************************************************
 *
 * Copyright (C) 2016-2025 IRCAD France
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

#include <data/validator/base.hpp>

namespace sight::data::validator::camera_set
{

/**
 * @brief Defines a validator which checks if the current camera_set contains two calibrated cameras and an extrinsic
 *        matrix.
 */
class stereo_camera : public sight::data::validator::base
{
public:

    SIGHT_DECLARE_CLASS(
        stereo_camera,
        sight::data::validator::base,
        sight::data::validator::factory::make<stereo_camera>
    )
    /// Destructor. Do nothing.
    ~stereo_camera() override = default;

    /**
     * @brief Checks if the current camera_set contains two calibrated Cameras and an extrinsic matrix.
     * @see data::validator::base::validate
     */
    sight::data::validator::return_t validate(const CSPTR(data::object)& _current_data) const
    override;
};

} // namespace sight::data::validator::camera_set
