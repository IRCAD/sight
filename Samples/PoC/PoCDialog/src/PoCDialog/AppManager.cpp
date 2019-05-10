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

#include "PoCDialog/AppManager.hpp"

#include <fwServices/registry/ActiveWorkers.hpp>
#include <fwServices/registry/ObjectService.hpp>

namespace PoCDialog
{

//------------------------------------------------------------------------------

AppManager::AppManager() noexcept
{

}

//------------------------------------------------------------------------------

AppManager::~AppManager() noexcept
{
}

//------------------------------------------------------------------------------

void AppManager::initialize()
{
    this->create();

    ::fwThread::Worker::sptr worker = ::fwThread::Worker::New();
    auto workerRegistry = ::fwServices::registry::ActiveWorkers::getDefault();
    workerRegistry->addWorker("AppManager-slots", worker);

    this->startServices();
}

//------------------------------------------------------------------------------

void AppManager::uninitialize()
{
    this->stopAndUnregisterServices();
}

//------------------------------------------------------------------------------

} // namespace PoCDialog
