/************************************************************************
 *
 * Copyright (C) 2009-2021 IRCAD France
 * Copyright (C) 2012-2021 IHU Strasbourg
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

#include "modules/utest/config.hpp"

#include <core/runtime/Plugin.hpp>

namespace sight::modules::utest
{

/**
 * @brief   This class is started when the module is loaded.
 */
struct MODULE_UTEST_CLASS_API Plugin : public core::runtime::Plugin
{
    /// Destructor. Do nothing.
    MODULE_UTEST_API ~Plugin() noexcept;

    /// Overrides start method. Service declarations (services declared in fwServices libraries ).
    MODULE_UTEST_API void start();

    /// Overrides initialize method
    MODULE_UTEST_API void initialize();

    /// Overrides uninitialize method
    MODULE_UTEST_API void uninitialize();

    /// Overrides stop method. Do nothing
    MODULE_UTEST_API void stop() noexcept;

};

} // namespace sight::modules::services
