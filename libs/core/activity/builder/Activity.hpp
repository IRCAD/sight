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

#include "activity/config.hpp"
#include "activity/IBuilder.hpp"

namespace sight::activity
{

namespace builder
{

/**
 * @brief Defines the default builder for the data Activity.
 */
class ACTIVITY_CLASS_API Activity : public activity::IBuilder
{
public:

    SIGHT_DECLARE_CLASS(Activity, activity::IBuilder, activity::builder::factory::New<Activity>);

    typedef std::string RegistryKeyType;

    /// Constructor. Do nothing.
    ACTIVITY_API Activity(activity::IBuilder::Key key);

    /// Destructor. Do nothing.
    ACTIVITY_API virtual ~Activity() = default;

    /**
     * @brief Build an Activity with required data present in currentSelection and defined in configuration.
     * returns specific data Activity for the specified Activity, or a
     * null pointer if the builder canceled the activity creation.
     * @param[in] activityInfo a structure which contains all the Activity configuration
     * @param[in] currentSelection a vector which contains current selected data.
     * @return Activity or null
     */
    ACTIVITY_API virtual data::Activity::sptr buildData(
        const activity::extension::ActivityInfo& activityInfo,
        const CSPTR(data::Vector)& currentSelection
    ) const override;
};

} // namespace builder

} // namespace sight::activity
