/************************************************************************
 *
 * Copyright (C) 2009-2021 IRCAD France
 * Copyright (C) 2012-2019 IHU Strasbourg
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

#include "activities/config.hpp"
#include "activities/registry/Activities.hpp"
#include "activities/validator/factory/new.hpp"
#include "activities/validator/registry/detail.hpp"

#include <core/base.hpp>

#include <data/ActivitySeries.hpp>

namespace sight::data
{
class Vector;
}

namespace sight::activities
{

/**
 * @brief Base class for all fwActivities validator's classes
 */
class ACTIVITIES_CLASS_API IValidator : public core::BaseObject
{

public:

    /**
     * @brief Defines validation result of an activity.
     * First element tells if the activity is validated or not by the IValidator implementation.
     * Second element may contains information about the result of activity validation.
     */
    typedef std::pair< bool, std::string > ValidationType;

    typedef activities::validator::factory::Key Key;

    /**
     * @brief Class used to register a class factory in factory registry.
     * This class defines also the object factory ( 'create' )
     *
     * @tparam T Factory product type
     */
    template <typename T>
    class Registrar
    {
    public:
        Registrar(std::string functorKey)
        {
            activities::validator::registry::get()->addFactory( functorKey,
                                                                &activities::validator::factory::New<T> );
        }
    };

    fwCoreClassMacro(IValidator, core::BaseObject)

    /**
     * @brief Performs validation of given data as inputs for the considered activity.
     * @param[in] activityInfo a structure which contains all the activity configuration
     * @param[in] currentSelection a vector which contains current selected data.
     * @return result of the validation
     *
     * @see IValidator::ValidationType
     */
    ACTIVITIES_API virtual ValidationType validate(
        const activities::registry::ActivityInfo& activityInfo,
        const CSPTR(data::Vector)& currentSelection ) const = 0;

};

} // namespace sight::activities
