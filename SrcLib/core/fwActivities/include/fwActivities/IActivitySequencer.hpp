/************************************************************************
 *
 * Copyright (C) 2019 IRCAD France
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

#include "fwActivities/config.hpp"
#include "fwActivities/IActivityLauncher.hpp"

#include <fwData/Composite.hpp>
#include <fwData/Object.hpp>

#include <fwMedData/SeriesDB.hpp>

namespace fwActivities
{

/**
 * @brief Base class for services that creates Activities sequentially
 */
class FWACTIVITIES_CLASS_API IActivitySequencer : public IActivityLauncher
{
public:

    /// Constructor. Do nothing.
    FWACTIVITIES_API IActivitySequencer();

    /// Destructor. Do nothing.
    FWACTIVITIES_API virtual ~IActivitySequencer();

protected:

    typedef std::vector< std::string > ActivitesType;
    typedef std::map< std::string, ::fwData::Object::sptr > RequirementsType;

    /**
     * @brief parse the activities contained in seriesDB and store their data
     *
     * @return Return the index on the last available activity
     *
     * @warning This method remove the series that are not in the list of activity to launch
     */
    FWACTIVITIES_API int parseActivities(const ::fwMedData::SeriesDB::sptr& seriesDB);

    /// Store the current activity data
    FWACTIVITIES_API void storeActivityData(const ::fwMedData::SeriesDB::sptr& seriesDB,
                                            const ::fwData::Composite::csptr& overrides = nullptr);

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
    FWACTIVITIES_API ::fwMedData::ActivitySeries::sptr getActivity(const ::fwMedData::SeriesDB::sptr& seriesDB,
                                                                   size_t index,
                                                                   const ::fwCom::SlotBase::sptr& slot         = nullptr,
                                                                   const ::fwData::Composite::csptr& overrides = nullptr);

    /// List of the activity to create.
    ActivitesType m_activityIds;

    /// Index of the current activity
    int m_currentActivity{-1};

    /// Map containing all the data produced by the activities. It is used to create the next one.
    RequirementsType m_requirements;

};

} // namespace fwActivities
