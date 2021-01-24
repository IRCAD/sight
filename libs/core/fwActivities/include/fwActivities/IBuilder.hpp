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

#include "fwActivities/builder/factory/new.hpp"
#include "fwActivities/builder/registry/detail.hpp"
#include "fwActivities/config.hpp"
#include "fwActivities/registry/Activities.hpp"

#include <core/base.hpp>

#include <fwMedData/ActivitySeries.hpp>

namespace sight::data
{
class Vector;
}

namespace fwActivities
{

/**
 * @brief Base class for all fwActivities builder's classes
 */
class FWACTIVITIES_CLASS_API IBuilder : public core::BaseObject
{

public:

    typedef ::fwActivities::builder::factory::Key Key;
    typedef ::fwMedData::ActivitySeries::ConfigIdType ConfigIdType;

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
            ::fwActivities::builder::registry::get()->addFactory( functorKey,
                                                                  &::fwActivities::builder::factory::New<T> );
        }
    };

    fwCoreClassMacro(IBuilder, core::BaseObject)

    /**
     * @brief Build an ActivitySeries with required data present in currentSelection and defined in configuration.
     * @param[in] activityInfo a structure which contains all the Activity configuration
     * @param[in] currentSelection a vector which contains current selected data.
     * @return specific data ActivitySeries for the specified Activity.
     */
    FWACTIVITIES_API virtual ::fwMedData::ActivitySeries::sptr buildData(
        const ::fwActivities::registry::ActivityInfo& activityInfo,
        const CSPTR(data::Vector)& currentSelection ) const = 0;

protected:

    FWACTIVITIES_API virtual SPTR(data::Vector) getType( const CSPTR(data::Vector)& currentSelection,
                                                         const std::string& type ) const;
};

} // namespace fwActivities
