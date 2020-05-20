/************************************************************************
 *
 * Copyright (C) 2016-2019 IRCAD France
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

#include "validators/config.hpp"

#include <fwActivities/IObjectValidator.hpp>

namespace validators
{
namespace CameraSeries
{
/**
 * @brief Defines a validator which checks if the current CameraSeries contains two calibrated cameras and an extrinsic
 *        matrix.
 */
class VALIDATORS_CLASS_API StereoCamera : public ::fwActivities::IObjectValidator
{
public:
    fwCoreClassMacro(StereoCamera, ::fwActivities::IObjectValidator,
                     ::fwActivities::validator::factory::New< StereoCamera >);

    /// Constructor. Do nothing.
    VALIDATORS_API StereoCamera(::fwActivities::IValidator::Key key);

    /// Destructor. Do nothing.
    VALIDATORS_API virtual ~StereoCamera();

    /**
     * @brief Checks if the current CameraSeries contains two calibrated Cameras and an extrinsic matrix.
     * @see ::fwActivities::IValidator::validate
     */
    VALIDATORS_API virtual IValidator::ValidationType validate(const CSPTR(::fwData::Object)& currentObject ) const
    override;
};

} // namespace CameraSeries
} // namespace validators
