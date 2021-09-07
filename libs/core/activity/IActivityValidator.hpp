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

#include "activity/config.hpp"
#include "activity/IValidator.hpp"

#include <core/base.hpp>

#include <data/ActivitySeries.hpp>

namespace sight::data
{

class ActivitySeries;

}

namespace sight::activity
{

/**
 * @brief Base class for all activity validator's classes.
 *
 * This validator allows to check if all the required data are valid for the activity.
 */
class ACTIVITY_CLASS_API IActivityValidator : public activity::IValidator
{
public:

    SIGHT_DECLARE_CLASS(IActivityValidator, activity::IValidator);

    /// Does nothing.
    ACTIVITY_API virtual ValidationType validate(
        const activity::extension::ActivityInfo&,
        const CSPTR(data::Vector)&
    ) const override
    {
        ValidationType validation;
        validation.first  = true;
        validation.second = "This validator does not validate this selection of data.";
        return validation;
    }

    /**
     * @brief Performs the validation of the given activity data.
     * @return pair <isValid, errorMsg> : errorMsg is empty if the data are valid else it contains detailed error.
     */
    ACTIVITY_API virtual ValidationType validate(const CSPTR(data::ActivitySeries)& activity) const = 0;

    /**
     * @brief Checks if all the required data are present in the activity series.
     * @return pair <isValid, errorMsg> : errorMsg is empty if the data are valid else it contains the list of missing
     *         (or not valid) data.
     */
    ACTIVITY_API ValidationType checkRequirements(const CSPTR(data::ActivitySeries)& activity) const;

    /**
     * @brief Checks if all the activity's AppConfig parameters are valid.
     *
     * It checks if the object described with "camp" path are present in the activity.
     * @return pair <isValid, errorMsg> : errorMsg is empty if the data are valid else it contains the list of missing
     *         (or not valid) data.
     */
    ACTIVITY_API ValidationType checkParameters(const CSPTR(data::ActivitySeries)& activity) const;

    /**
     * @brief Calls the object validator if it is defined.
     * @param object object to validate
     * @param validatorImpl implementation of the validator to apply (if it is empty, check is assumed as valid).
     * @return pair <isValid, errorMsg> : errorMsg is empty if the object is valid else it contains the detailed error.
     * @see activity::IObjectValidator
     */
    ACTIVITY_API ValidationType checkObject(
        const CSPTR(data::Object)& object,
        const std::string& validatorImpl
    ) const;
};

} // namespace sight::activity
