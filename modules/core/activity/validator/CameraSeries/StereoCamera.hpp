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

#include "modules/activity/config.hpp"

#include <modules/activity/IObjectValidator.hpp>

namespace sight::modules::activity::validator
{
namespace CameraSeries
{
/**
 * @brief Defines a validator which checks if the current CameraSeries contains two calibrated cameras and an extrinsic
 *        matrix.
 */
class MODULE_ACTIVITY_CLASS_API StereoCamera : public sight::modules::activity::IObjectValidator
{
public:
    fwCoreClassMacro(StereoCamera, modules::activity::IObjectValidator,
                     modules::activity::validator::factory::New< StereoCamera >)

    /// Constructor. Do nothing.
    MODULE_ACTIVITY_API StereoCamera(modules::activity::IValidator::Key key);

    /// Destructor. Do nothing.
    MODULE_ACTIVITY_API virtual ~StereoCamera();

    /**
     * @brief Checks if the current CameraSeries contains two calibrated Cameras and an extrinsic matrix.
     * @see modules::activity::IValidator::validate
     */
    MODULE_ACTIVITY_API virtual IValidator::ValidationType validate(const CSPTR(data::Object)& currentObject ) const
    override;
};

} // namespace CameraSeries
} // namespace sight::modules::activity::validator
