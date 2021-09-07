/************************************************************************
 *
 * Copyright (C) 2009-2021 IRCAD France
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

#include "modules/debug/config.hpp"

#include <ui/base/IAction.hpp>

namespace sight::module::debug
{

namespace action
{

/// Implements an action for show metrics about Sight factories (number of data, message and service in factories).
class MODULE_DEBUG_CLASS_API fwMetrics : public ::sight::ui::base::IAction
{
public:

    SIGHT_DECLARE_SERVICE(fwMetrics, sight::ui::base::IAction);

    /// Does nothing
    MODULE_DEBUG_API fwMetrics() noexcept;

    /// Does nothing
    MODULE_DEBUG_API virtual ~fwMetrics() noexcept;

protected:

    /// Calls classic IAction methods to configure
    void configuring() override;

    /// Calls classic IAction methods to start
    void starting() override;

    /// Show metrics in a dialog message box
    void updating() override;

    /// Calls classic IAction methods to stop
    void stopping() override;
};

} // namespace action

} // namespace sight::module::debug
