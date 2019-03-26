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

#include "fwGuiQml/config.hpp"

#include <fwRuntime/Plugin.hpp>

namespace fwGuiQml
{
/**
 * @brief   This class is started when the bundle is loaded.
 */
class FWGUIQML_CLASS_API Plugin : public ::fwRuntime::Plugin
{
public:
    /// Constructor.
    FWGUIQML_CLASS_API Plugin() noexcept;

    /// Destructor. Do nothing.
    FWGUIQML_CLASS_API ~Plugin() noexcept;

    /// Overrides start method
    FWGUIQML_CLASS_API void start();

    FWGUIQML_CLASS_API void initialize();

    FWGUIQML_CLASS_API void uninitialize() noexcept;

    /// Overrides stop method. Do nothing
    FWGUIQML_CLASS_API void stop() noexcept;

};

} // namespace fwGuiQml
