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

#include "calibrationActivity/config.hpp"

#include <fwActivities/IObjectValidator.hpp>

#include <fwCore/base.hpp>

namespace calibrationActivity
{
namespace validator
{

/**
 * @brief Defines a validator which checks that the current data String contains a list of tag identifiers separated by
 *        a ',' (for example: "101, 102, 103").
 *
 */
class CALIBRATIONACTIVITY_CLASS_API TagId : public ::fwActivities::IObjectValidator
{
public:
    fwCoreClassMacro(TagId, ::fwActivities::IValidator, ::fwActivities::validator::factory::New< TagId >);

    /// Constructor. Do nothing.
    CALIBRATIONACTIVITY_API TagId(::fwActivities::IValidator::Key key);

    /// Destructor. Do nothing.
    CALIBRATIONACTIVITY_API virtual ~TagId();

    CALIBRATIONACTIVITY_API virtual ValidationType validate(const CSPTR(::fwData::Object)& currentData ) const override;
};

} // namespace validator
} // namespace calibrationActivity
