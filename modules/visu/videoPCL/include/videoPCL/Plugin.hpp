/************************************************************************
 *
 * Copyright (C) 2017-2021 IRCAD France
 * Copyright (C) 2017-2020 IHU Strasbourg
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

#include "videoPCL/config.hpp"

#include <core/runtime/Plugin.hpp>

namespace videoPCL
{
struct Plugin : public core::runtime::Plugin
{
    /**
     * @brief Destructor
     */
    ~Plugin() noexcept;

    /**
     * @brief Start method.
     *
     * @exception core::runtime::RuntimeException.
     * This method is used by runtime in order to initialize the module.
     */
    void start();

    /**
     * @brief Stop method.
     *
     * This method is used by runtime in order to close the module.
     */
    void stop() noexcept;
};

} // namespace videoPCL
