/************************************************************************
 *
 * Copyright (C) 2009-2022 IRCAD France
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

#include "activity/builder/factory/new.hpp"
#include "activity/builder/registry/detail.hpp"
#include "activity/config.hpp"
#include "activity/extension/Activity.hpp"

#include <core/base.hpp>

#include <data/ActivitySeries.hpp>

namespace sight::data
{

class Vector;

} // namespace sight::data

namespace sight::activity
{

/**
 * @brief Base class for all activity builder's classes.
 */
class ACTIVITY_CLASS_API IBuilder : public core::BaseObject
{
public:

    typedef activity::builder::factory::Key Key;
    typedef data::ActivitySeries::ConfigIdType ConfigIdType;

    /**
     * @brief Class used to register a class factory in factory registry.
     * This class defines also the object factory ( 'create' )
     *
     * @tparam T Factory product type
     */
    template<typename T>
    class Registry
    {
    public:

        Registry(std::string functorKey)
        {
            activity::builder::registry::get()->addFactory(
                functorKey,
                &activity::builder::factory::New<T>
            );
        }
    };

    SIGHT_DECLARE_CLASS(IBuilder, core::BaseObject);

    /**
     * @brief Build an ActivitySeries with required data present in currentSelection and defined in configuration.
     * @param[in] activityInfo a structure which contains all the Activity configuration
     * @param[in] currentSelection a vector which contains current selected data.
     * @return specific data ActivitySeries for the specified Activity.
     */
    ACTIVITY_API virtual data::ActivitySeries::sptr buildData(
        const activity::extension::ActivityInfo& activityInfo,
        const CSPTR(data::Vector)& currentSelection
    ) const = 0;

protected:

    ACTIVITY_API virtual SPTR(data::Vector) getType(
        const CSPTR(data::Vector) & currentSelection,
        const std::string& type
    ) const;
};

} // namespace sight::activity
