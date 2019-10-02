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

#pragma once

#include "ExActivitiesQml/config.hpp"

#include <fwRuntime/Plugin.hpp>

namespace ExActivitiesQml
{

/**
 * @brief   This class is started when the bundle is loaded.
 */
class EXACTIVITIESQML_CLASS_API Plugin : public ::fwRuntime::Plugin
{
public:
    /// Constructor.
    EXACTIVITIESQML_API Plugin() noexcept;

    /// Destructor. Do nothing.
    EXACTIVITIESQML_API ~Plugin() noexcept;

    /// Overrides start method. .
    EXACTIVITIESQML_API void start();

    /// Overrides stop method. Do nothing
    EXACTIVITIESQML_API void stop() noexcept;

    EXACTIVITIESQML_API void initialize();

    EXACTIVITIESQML_API void uninitialize() noexcept;

};

} // namespace ExActivitiesQml
