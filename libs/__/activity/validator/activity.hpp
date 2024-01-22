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

#include "activity/config.hpp"
#include "activity/validator/base.hpp"

#include <core/base.hpp>

#include <data/activity.hpp>

namespace sight::data
{

class Activity;

} // namespace sight::data

namespace sight::activity::validator
{

/**
 * @brief Base class for all activity validator's classes.
 *
 * This validator allows to check if all the required data are valid for the activity.
 */
class ACTIVITY_CLASS_API activity : public validator::base
{
public:

    SIGHT_DECLARE_CLASS(activity, validator::base);

    /// Does nothing.
    ACTIVITY_API return_t validate(
        const extension::activity_info& /*activity_info*/,
        const CSPTR(data::vector)&
        /*currentSelection*/
    ) const override
    {
        return_t validation;
        validation.first  = true;
        validation.second = "This validator does not validate this selection of data.";
        return validation;
    }

    /**
     * @brief Performs the validation of the given activity data.
     * @return pair <isValid, errorMsg> : errorMsg is empty if the data are valid else it contains detailed error.
     */
    ACTIVITY_API virtual return_t validate(const CSPTR(data::activity)& _activity) const = 0;

    /**
     * @brief Checks if all the required data are present in the activity.
     * @return pair <isValid, errorMsg> : errorMsg is empty if the data are valid else it contains the list of missing
     *         (or not valid) data.
     */
    static ACTIVITY_API return_t check_requirements(const CSPTR(data::activity)& _activity);

    /**
     * @brief Calls the object validator if it is defined.
     * @param _object object to validate
     * @param _validator_impl implementation of the validator to apply (if it is empty, check is assumed as valid).
     * @return pair <isValid, errorMsg> : errorMsg is empty if the object is valid else it contains the detailed error.
     * @see activity::validator::object
     */
    static ACTIVITY_API return_t check_object(
        const CSPTR(data::object)& _object,
        const std::string& _validator_impl
    );
};

} // namespace sight::activity::validator
