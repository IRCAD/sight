/************************************************************************
 *
 * Copyright (C) 2019-2021 IRCAD France
 * Copyright (C) 2019 IHU Strasbourg
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
#include "activity/IActivityLauncher.hpp"

#include <data/Composite.hpp>
#include <data/Object.hpp>
#include <data/SeriesDB.hpp>

namespace sight::activity
{

/**
 * @brief Base class for services that creates Activity instances sequentially.
 */
class ACTIVITY_CLASS_API IActivitySequencer : public IActivityLauncher
{
public:

    /// Constructor. Do nothing.
    ACTIVITY_API IActivitySequencer();

    /// Destructor. Do nothing.
    ACTIVITY_API virtual ~IActivitySequencer();

protected:

    typedef std::vector<std::string> ActivitesType;
    typedef std::map<std::string, data::Object::sptr> RequirementsType;

    /**
     * @brief parse the activities contained in seriesDB and store their data
     *
     * @return Return the index on the last available activity
     *
     * @warning This method remove the series that are not in the list of activity to launch
     */
    ACTIVITY_API int parseActivities(data::SeriesDB& seriesDB);

    /// Store the data of the activity at the given index
    ACTIVITY_API void storeActivityData(
        const data::SeriesDB& seriesDB,
        int index,
        const data::Composite::csptr& overrides = nullptr
    );

    /**
     * @brief Return the activity corresponding to the given index in the activity list. If the activity does not exist,
     * it is generated and pushed in the seriesBD.
     *
     * Uses the data of the previously launched activities to create/update the current activity
     *
     * @param seriesDB SeriesDB containing all the activities to be launched sequentially
     * @param index index of the activity to retrieve
     * @param slot slot to block in case the activity is created. It is usefull if the service listen notification on
     * the seriesDB
     * @param overrides Composite that contains data to override the previouly stored data (from the other activities)
     */
    ACTIVITY_API data::ActivitySeries::sptr getActivity(
        data::SeriesDB& seriesDB,
        size_t index,
        const core::com::SlotBase::sptr& slot   = nullptr,
        const data::Composite::csptr& overrides = nullptr
    );

    /// List of the activity to create.
    ActivitesType m_activityIds;

    /// Index of the current activity
    int m_currentActivity {-1};

    /// Map containing all the data produced by the activities. It is used to create the next one.
    RequirementsType m_requirements;
};

} // namespace sight::activity
