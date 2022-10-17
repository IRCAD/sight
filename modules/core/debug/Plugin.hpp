/************************************************************************
 *
 * Copyright (C) 2009-2022 IRCAD France
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

#include <core/runtime/Plugin.hpp>

namespace sight::module::debug
{

/// Class called when the module is loaded and stopped.
struct Plugin : public core::runtime::Plugin
{
    /// Destructor
    ~Plugin() noexcept override;

    // Install a callback which print the backtrace on a SIGSEV and SIGUSR1 signal
    void start() override;

    // Overrides
    void stop() noexcept override;
};

} // namespace sight::module::debug
