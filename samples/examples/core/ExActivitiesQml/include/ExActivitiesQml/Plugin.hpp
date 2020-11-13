/************************************************************************
 *
 * Copyright (C) 2019-2020 IRCAD France
 * Copyright (C) 2019-2020 IHU Strasbourg
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

#include "ExActivitiesQml/config.hpp"

#include <fwRuntime/Plugin.hpp>

namespace ExActivitiesQml
{

/// This class is started when the module is loaded.
class EXACTIVITIESQML_CLASS_API Plugin : public ::fwRuntime::Plugin
{

public:

    /// Destroys the plugin.
    EXACTIVITIESQML_API ~Plugin() noexcept override;

    /// Starts the plugin, registers qml files.
    EXACTIVITIESQML_API void start() override;

    /// Stops the plugin, does nothing here.
    EXACTIVITIESQML_API void stop() noexcept override;

    /// Load the main qml application.
    EXACTIVITIESQML_API void initialize() override;

    /// Does nothing here.
    EXACTIVITIESQML_API void uninitialize() override;

};

} // namespace ExActivitiesQml.
