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

#include "ExActivitiesQml/ImageReadingManager.hpp"

#include <fwCom/Signal.hxx>
#include <fwCom/Slot.hxx>

#include <fwData/factory/new.hpp>

#include <fwMedData/SeriesDB.hpp>

#include <fwServices/op/Add.hpp>
#include <fwServices/registry/ActiveWorkers.hpp>
#include <fwServices/registry/ObjectService.hpp>
#include <fwServices/registry/Proxy.hpp>

#include <QDebug>

//------------------------------------------------------------------------------

ImageReadingManager::ImageReadingManager() noexcept
{
    s_SERIES_DB                        = this->getID(s_SERIES_DB);
    s_ACTIVITY_CREATED_CHANNEL         = this->getID(s_ACTIVITY_CREATED_CHANNEL);
    s_ACTIVITY_LAUNCHED_CHANNEL        = this->getID(s_ACTIVITY_LAUNCHED_CHANNEL);
    s_ENABLED_PREVIOUS_CHANNEL_CHANNEL = this->getID(s_ENABLED_PREVIOUS_CHANNEL_CHANNEL);
    s_ENABLED_NEXT_CHANNEL_CHANNEL     = this->getID(s_ENABLED_NEXT_CHANNEL_CHANNEL);
    s_PREVIOUS_CHANNEL                 = this->getID(s_PREVIOUS_CHANNEL);
    s_NEXT_CHANNEL                     = this->getID(s_NEXT_CHANNEL);
}

//------------------------------------------------------------------------------

ImageReadingManager::~ImageReadingManager() noexcept
{
}

//------------------------------------------------------------------------------

void ImageReadingManager::initialize()
{
    this->create();

    this->startServices();
}

//------------------------------------------------------------------------------

void ImageReadingManager::uninitialize()
{
    // stop the started services and unregister all the services
    this->stopAndUnregisterServices();
}

//------------------------------------------------------------------------------
