/************************************************************************
 *
 * Copyright (C) 2009-2020 IRCAD France
 * Copyright (C) 2012-2020 IHU Strasbourg
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

#include "fwServices/config.hpp"

#include <fwCore/mt/types.hpp>

#include <fwThread/ActiveWorkers.hpp>

namespace fwServices
{
namespace registry
{

/**
 * @brief This class to register active worker in the system, creates a default worker
 * @deprecated  Please now use ::fwThread::ActiveWorkers instead
 */
[[deprecated]] typedef ::fwThread::ActiveWorkers ActiveWorkers;

} // namespace registry
} // namespace fwServices
