/************************************************************************
 *
 * Copyright (C) 2014-2023 IRCAD France
 * Copyright (C) 2014-2020 IHU Strasbourg
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

#include "ex_notifications/config.hpp"

#include <core/runtime/plugin.hpp>

/**
 * Do not mark `ex_notifications` as incorrect.
 * cspell:ignore ex_notifications
 */

namespace ex_notifications
{

/// This class is started when the module is loaded.
class EX_NOTIFICATIONS_CLASS_API plugin : public sight::core::runtime::plugin
{
public:

    /// Destroys the plugin.
    EX_NOTIFICATIONS_API ~plugin() noexcept override;

    /// Adds the default worker.
    EX_NOTIFICATIONS_API void start() override;

    /// Does nothing here.
    EX_NOTIFICATIONS_API void stop() noexcept override;
};

} // namespace ex_notifications
