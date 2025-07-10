/************************************************************************
 *
 * Copyright (C) 2024 IRCAD France
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

#include "sight/ex_progress_bar/config.hpp"

#include <core/runtime/plugin.hpp>

namespace ex_progress_bar
{

/// This class is started when the module is loaded.
class SIGHT_EX_PROGRESS_BAR_CLASS_API plugin : public sight::core::runtime::plugin
{
public:

    /// Destroys the plugin.
    SIGHT_EX_PROGRESS_BAR_API ~plugin() noexcept override = default;

    /// Adds the default worker.
    SIGHT_EX_PROGRESS_BAR_API void start() override;

    /// Does nothing here.
    SIGHT_EX_PROGRESS_BAR_API void stop() noexcept override;
};

} // namespace ex_progress_bar.
