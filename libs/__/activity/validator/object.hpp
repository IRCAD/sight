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

#include "activity/config.hpp"
#include "activity/validator/base.hpp"

#include <core/base.hpp>

namespace sight::data
{

class Object;

} // namespace sight::data

namespace sight::activity::validator
{

/**
 * @brief Base class for all activity data validator's classes.
 *
 * This validator works on one type of data. The validated data can be a single data, or a Vector or a Composite
 * containing multiple data of the same type.
 */
class ACTIVITY_CLASS_API object : public validator::base
{
public:

    SIGHT_DECLARE_CLASS(object, validator::base);

    /// Does nothing.
    ACTIVITY_API return_t validate(
        const extension::ActivityInfo& /*activityInfo*/,
        const CSPTR(data::Vector)&
        /*currentSelection*/
    ) const override
    {
        return_t validation;
        validation.first  = true;
        validation.second = "This validator does not validate this selection of data.";
        return validation;
    }

    /**
     * @brief Performs the validation of the given data.
     * @note  This data could be a single data, or a Vector or a Composite of one type of data.
     */
    ACTIVITY_API virtual return_t validate(const CSPTR(data::Object)& currentData) const = 0;
};

} // namespace sight::activity::validator
