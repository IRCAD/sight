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

#include "Tuto01BasicQml/config.hpp"

#include <core/runtime/Plugin.hpp>

namespace Tuto01BasicQml
{

/**
 * @brief   This class is started when the module is loaded.
 */
class TUTO01BASICQML_CLASS_API Plugin : public sight::core::runtime::Plugin
{
public:

    /// Constructor.
    TUTO01BASICQML_API Plugin() noexcept;

    /// Destructor. Do nothing.
    TUTO01BASICQML_API ~Plugin() noexcept;

    /// Overrides start method. .
    TUTO01BASICQML_API void start();

    /// Overrides stop method. Do nothing
    TUTO01BASICQML_API void stop() noexcept;

    TUTO01BASICQML_API void initialize();

    TUTO01BASICQML_API void uninitialize() noexcept;
};

} // namespace Tuto01BasicQml
