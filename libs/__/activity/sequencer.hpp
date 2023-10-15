/************************************************************************
 *
 * Copyright (C) 2019-2023 IRCAD France
 * Copyright (C) 2019-2021 IHU Strasbourg
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
#include "activity/launcher.hpp"

#include <data/activity_set.hpp>
#include <data/composite.hpp>

namespace sight::activity
{

/**
 * @brief Base class for services that creates Activity instances sequentially.
 */
class ACTIVITY_CLASS_API sequencer : public launcher
{
public:

    /// Constructor. Do nothing.
    ACTIVITY_API sequencer() = default;

    /// Destructor. Do nothing.
    ACTIVITY_API ~sequencer() override = default;

protected:

    /**
     * @brief parse the activities contained in activity_set and store their data
     *
     * @return Return the index on the last available activity
     *
     * @warning This method remove the activity that are not in the list of activity to launch
     */
    ACTIVITY_API int parseActivities(data::activity_set& activity_set);

    /// Store the data of the activity at the given index
    ACTIVITY_API void storeActivityData(
        const data::activity_set& activity_set,
        std::size_t index,
        const data::composite::csptr& overrides = nullptr
    );

    /**
     * @brief Return the activity corresponding to the given index in the activity list. If the activity does not exist,
     * it is generated and pushed in the activity set.
     *
     * Uses the data of the previously launched activities to create/update the current activity
     *
     * @param activity_set ActivitySet containing all the activities to be launched sequentially
     * @param index index of the activity to retrieve
     * @param slot slot to block in case the activity is created. It is usefull if the service listen notification on
     * the activity_set
     */
    ACTIVITY_API data::activity::sptr getActivity(
        data::activity_set& activity_set,
        std::size_t index,
        const core::com::slot_base::sptr& slot = nullptr
    );

    /**
     * @brief Remove the activity with index and its requirements and all the following activities.
     *
     * This is used to clear the activity set and requirement when going backward.
     *
     * @param activity_set ActivitySet containing all the activities
     * @param index the activity in index and all the following will be removed
     */
    ACTIVITY_API void removeLastActivities(data::activity_set& activity_set, std::size_t index);

    /**
     * @brief Reset all data created by activities (create="true", minOccurs="0") at index and beyond.
     *
     * This is used to clean activities when previous one changed their requirements.
     *
     * @param activity_set ActivitySet containing all the activities
     * @param index the activity in index and all the following will be cleaned
     */
    ACTIVITY_API void cleanRequirements(std::size_t index);

    /// List of the activity to create.
    std::vector<std::string> m_activityIds;

    /// Index of the current activity
    int m_currentActivity {-1};

    /// Map containing all the data produced by the activities. It is used to create the next one.
    std::map<std::string, data::object::sptr> m_requirements;
};

} // namespace sight::activity
