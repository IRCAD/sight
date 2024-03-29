/************************************************************************
 *
 * Copyright (C) 2009-2023 IRCAD France
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

#include <ui/__/action.hpp>

namespace sight::module::debug::action
{

/// Implements an action for show metrics about Sight factories (number of data, message and service in factories).
class MODULE_DEBUG_CLASS_API metrics : public sight::ui::action
{
public:

    SIGHT_DECLARE_SERVICE(metrics, sight::ui::action);

    /// Does nothing
    MODULE_DEBUG_API metrics() noexcept;

    /// Does nothing
    MODULE_DEBUG_API ~metrics() noexcept override;

protected:

    /// Calls classic action methods to configure
    void configuring() override;

    /// Calls classic action methods to start
    void starting() override;

    /// Show metrics in a dialog message box
    void updating() override;

    /// Calls classic action methods to stop
    void stopping() override;
};

} // namespace sight::module::debug::action
