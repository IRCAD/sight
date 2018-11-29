/************************************************************************
 *
 * Copyright (C) 2015 IRCAD France
 * Copyright (C) 2015 IHU Strasbourg
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

#ifndef __FWACTIVITIES_REGISTRY_ACTIVITYMSG_HPP__
#define __FWACTIVITIES_REGISTRY_ACTIVITYMSG_HPP__

#include "fwActivities/config.hpp"
#include "fwActivities/registry/Activities.hpp"

#include <fwMedData/ActivitySeries.hpp>

namespace fwActivities
{

namespace registry
{

/**
 * @class ActivityMsg
 * @brief Activity information sent by signal to launch new activities in a tab.
 */
class FWACTIVITIES_CLASS_API ActivityMsg
{

public:

    typedef ::fwActivities::registry::ActivityAppConfig::ActivityAppConfigParamsType ParametersType;
    typedef std::map<std::string, std::string> ReplaceMapType;


    FWACTIVITIES_API ActivityMsg(const ::fwMedData::ActivitySeries::sptr &series,
                                 const ::fwActivities::registry::ActivityInfo & info,
                                 const ParametersType& parameters);

    FWACTIVITIES_API virtual ~ActivityMsg()
    {
    }

    /// Return if the activity can be closed
    bool isClosable() const
    {
        return m_closable;
    }

    /// Return activity title
    const std::string &getTitle() const
    {
        return m_title;
    }

    /// Return tab identifier
    const std::string &getTabID() const
    {
        return m_tabID;
    }

    /// Return appConfig identifier
    const std::string &getAppConfigID() const
    {
        return m_appConfigID;
    }

    /// Return tab information
    const std::string &getTabInfo() const
    {
        return m_tabInfo;
    }

    /// Return activity icon path
    const std::string &getIconPath() const
    {
        return m_iconPath;
    }

    /// Return tooltip
    const std::string &getToolTip() const
    {
        return m_tooltip;
    }

    /// Return activity series
    const ::fwMedData::ActivitySeries::sptr &getActivitySeries() const
    {
        return m_series;
    }

    /// Return the map of the string association to replace in config
    const ReplaceMapType & getReplaceMap() const
    {
        return m_replaceMap;
    }

private:

    bool m_closable; /// Flag if activity is closable.
    std::string m_title; ///< Activity title
    std::string m_tabID; ///< Tab identifier
    std::string m_appConfigID; ///< AppConfig id
    std::string m_tabInfo; ///< tab information
    std::string m_iconPath; ///< icon path
    std::string m_tooltip; ///< tab tooltip
    ::fwMedData::ActivitySeries::sptr m_series; ///< activity series
    ReplaceMapType m_replaceMap; ///< map containing string to replace in activity configuration.
};

} // namespace registry

} // namespace fwActivities

#endif // __FWACTIVITIES_REGISTRY_ACTIVITYMSG_HPP__

