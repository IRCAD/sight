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

#include "ExActivitiesQml/config.hpp"

#include <fwCom/Signal.hpp>
#include <fwCom/Signal.hxx>

#include <fwMedData/SeriesDB.hpp>

#include <fwServices/AppManager.hpp>
#include <fwServices/IService.hpp>

#include <QObject>

/**
 * @brief   This class is started when the bundles is loaded.
 */
class EXACTIVITIESQML_CLASS_API AppManager : public QObject,
                                             public ::fwServices::AppManager,
                                             public ::fwCom::HasSignals
{
Q_OBJECT
public:
    /// Constructor.
    EXACTIVITIESQML_API AppManager() noexcept;

    /// Destructor. Do nothing.
    EXACTIVITIESQML_API ~AppManager() noexcept;

public Q_SLOTS:

    /// Initialize the manager
    void initialize();

    /// Slot: Create the previous activity series
    void previous(bool prev);

    /// Slot: Create the next activity series
    void next(bool next);

    /// Uninitialize the manager
    void uninitialize();

    /// Retrieves the services instanciated in Qml
    void onServiceCreated(const QVariant& obj);

private:

    ::fwServices::IService::sptr m_activitySequencer;

    ::fwMedData::SeriesDB::sptr m_seriesDB;

    ::fwServices::IService::ConfigType m_activityViewConfig;

    /// Identifier for the object and channel, is unique for an instance of the AppManager
    std::string s_SERIES_DB{"seriesDB"};
    std::string s_ACTIVITY_CREATED_CHANNEL{"activityCreated"};
    std::string s_ACTIVITY_LAUNCHED_CHANNEL        {"activityLaunched"};
    std::string s_ENABLED_PREVIOUS_CHANNEL_CHANNEL {"enabledPreviousChannel"};
    std::string s_ENABLED_NEXT_CHANNEL_CHANNEL     {"enabledNextChannel"};
    std::string s_PREVIOUS_CHANNEL                 {"Previous"};
    std::string s_NEXT_CHANNEL                     {"Next"};
};
