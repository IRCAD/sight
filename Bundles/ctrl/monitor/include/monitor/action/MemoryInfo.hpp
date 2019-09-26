/************************************************************************
 *
 * Copyright (C) 2009-2019 IRCAD France
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

#include "monitor/config.hpp"

#include <fwGui/IActionSrv.hpp>

namespace monitor
{
namespace action
{

/// Implements an action to show in a message box few memory information
class MONITOR_CLASS_API MemoryInfo : public ::fwGui::IActionSrv
{

public:

    fwCoreServiceMacro(MemoryInfo,  ::fwGui::IActionSrv );

    /// Does nothing
    MONITOR_API MemoryInfo() noexcept;

    /// Does nothing
    MONITOR_API virtual ~MemoryInfo() noexcept;

protected:

    /// Calls classic IAction methods to configure
    void configuring() override;

    /// Calls classic IAction methods to start
    void starting() override;

    /// Shows in a message box few memory information
    void updating() override;

    /// Calls classic IAction methods to stop
    void stopping() override;

};

} // namespace action
} // namespace monitor
