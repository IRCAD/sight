/************************************************************************
 *
 * Copyright (C) 2009-2019 IRCAD France
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

#include "fwActivities/config.hpp"
#include "fwActivities/registry/Activities.hpp"
#include "fwActivities/validator/factory/new.hpp"
#include "fwActivities/validator/registry/detail.hpp"

#include <fwCore/base.hpp>

#include <fwMedData/ActivitySeries.hpp>

namespace fwData
{
class Vector;
}

namespace fwActivities
{

/**
 * @brief Base class for all fwActivities validator's classes
 */
class FWACTIVITIES_CLASS_API IValidator : public ::fwCore::BaseObject
{

public:

    /**
     * @brief Defines validation result of an activity.
     * First element tells if the activity is validated or not by the IValidator implementation.
     * Second element may contains information about the result of activity validation.
     */
    typedef std::pair< bool, std::string > ValidationType;

    typedef ::fwActivities::validator::factory::Key Key;

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
            ::fwActivities::validator::registry::get()->addFactory( functorKey,
                                                                    &::fwActivities::validator::factory::New<T> );
        }
    };

    fwCoreClassMacro(IValidator, ::fwCore::BaseObject);

    /**
     * @brief Performs validation of given data as inputs for the considered activity.
     * @param[in] activityInfo a structure which contains all the activity configuration
     * @param[in] currentSelection a vector which contains current selected data.
     * @return result of the validation
     *
     * @see IValidator::ValidationType
     */
    FWACTIVITIES_API virtual ValidationType validate(
        const ::fwActivities::registry::ActivityInfo& activityInfo,
        const CSPTR(::fwData::Vector)& currentSelection ) const = 0;

};

} // namespace fwActivities
