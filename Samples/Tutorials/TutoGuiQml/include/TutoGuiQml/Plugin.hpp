/************************************************************************
 *
 * Copyright (C) 2018 IRCAD France
 * Copyright (C) 2018 IHU Strasbourg
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

#include "TutoGuiQml/config.hpp"

#include <fwRuntime/Plugin.hpp>

namespace TutoGuiQml
{

/**
 * @brief   This class is started when the bundle is loaded.
 */
class TUTOGUIQML_CLASS_API Plugin : public ::fwRuntime::Plugin
{
public:
    /// Constructor.
    TUTOGUIQML_API Plugin() noexcept;

    /// Destructor. Do nothing.
    TUTOGUIQML_API ~Plugin() noexcept;

    /// Overrides start method. .
    TUTOGUIQML_API void start();

    /// Overrides stop method. Do nothing
    TUTOGUIQML_API void stop() noexcept;

    TUTOGUIQML_API void initialize();

    TUTOGUIQML_API void uninitialize() noexcept;
};

} // namespace TutoGuiQml
