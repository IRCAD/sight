/************************************************************************
 *
 * Copyright (C) 2021-2022 IRCAD France
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

namespace sight::module::io::session
{

class Plugin : public core::runtime::Plugin
{
public:

    //! @brief Destructor.
    ~Plugin() noexcept override;

    /**
     * @brief Start method.
     *
     * This method is used by runtime in order to initialize the module.
     *
     * @exception core::runtime::RuntimeException
     */
    void start() override;

    /**
     * @brief Stop method.
     *
     * This method is used by runtime in order to close the module.
     */
    void stop() noexcept override;
};

} // namespace sight::module::io::session
