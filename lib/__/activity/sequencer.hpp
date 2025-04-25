/************************************************************************
 *
 * Copyright (C) 2019-2024 IRCAD France
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

#include <sight/activity/config.hpp>

#include "activity/launcher.hpp"

#include <data/activity_set.hpp>
#include <data/map.hpp>

namespace sight::activity
{

/**
 * @brief Base class for services that creates Activity instances sequentially.
 */
class SIGHT_ACTIVITY_CLASS_API sequencer : public launcher
{
public:

    /// Constructor. Do nothing.
    SIGHT_ACTIVITY_API sequencer() = default;

    /// Destructor. Do nothing.
    SIGHT_ACTIVITY_API ~sequencer() override = default;

protected:

    /**
     * @brief parse the activities contained in activity_set and store their data
     *
     * @return Return the index on the last available activity
     *
     * @warning This method remove the activity that are not in the list of activity to launch
     */
    SIGHT_ACTIVITY_API int parse_activities(data::activity_set& _activity_set);

    /// Store the data of the activity at the given index
    SIGHT_ACTIVITY_API void store_activity_data(
        const data::activity_set& _activity_set,
        std::size_t _index,
        const data::map::csptr& _overrides = nullptr
    );

    /**
     * @brief Return the activity corresponding to the given index in the activity list. If the activity does not exist,
     * it is generated and pushed in the activity set.
     *
     * Uses the data of the previously launched activities to create/update the current activity
     *
     * @param _activity_set ActivitySet containing all the activities to be launched sequentially
     * @param _index index of the activity to retrieve
     * @param _slot slot to block in case the activity is created. It is useful if the service listen notification on
     * the activity_set
     */
    SIGHT_ACTIVITY_API data::activity::sptr get_activity(
        data::activity_set& _activity_set,
        std::size_t _index,
        const core::com::slot_base::sptr& _slot = nullptr
    );

    /**
     * @brief Remove the activity with index and its requirements and all the following activities.
     *
     * This is used to clear the activity set and requirement when going backward.
     *
     * @param _activity_set ActivitySet containing all the activities
     * @param _index the activity in index and all the following will be removed
     */
    SIGHT_ACTIVITY_API void remove_last_activities(data::activity_set& _activity_set, std::size_t _index);

    /**
     * @brief Reset all data created by activities (create="true", minOccurs="0") at index and beyond.
     *
     * This is used to clean activities when previous one changed their requirements.
     */
    SIGHT_ACTIVITY_API void reset_requirements();

    /// List of the activity to create.
    std::vector<std::string> m_activity_ids;

    /// Index of the current activity
    int m_current_activity {-1};

    /// Map containing all the data produced by the activities. It is used to create the next one.
    std::map<std::string, data::object::sptr> m_requirements;
};

} // namespace sight::activity
