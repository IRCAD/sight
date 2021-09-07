/************************************************************************
 *
 * Copyright (C) 2018-2021 IRCAD France
 * Copyright (C) 2018-2020 IHU Strasbourg
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

#include "Tuto06GuiQml/config.hpp"

#include <core/runtime/Plugin.hpp>

namespace sight::Tuto06GuiQml
{

/**
 * @brief   This class is started when the module is loaded.
 */
class TUTO06GUIQML_CLASS_API Plugin : public sight::core::runtime::Plugin
{
public:

    /// Constructor.
    TUTO06GUIQML_API Plugin() noexcept;

    /// Destructor. Do nothing.
    TUTO06GUIQML_API ~Plugin() noexcept;

    /// Overrides start method. .
    TUTO06GUIQML_API void start();

    /// Overrides stop method. Do nothing
    TUTO06GUIQML_API void stop() noexcept;

    TUTO06GUIQML_API void initialize();

    TUTO06GUIQML_API void uninitialize() noexcept;
};

} // namespace sight::Tuto06GuiQml
