/************************************************************************
 *
 * Copyright (C) 2009-2023 IRCAD France
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

#include <core/runtime/plugin.hpp>

namespace sight::module::service
{

/**
 * @brief   This class is started when the module is loaded.
 */
struct plugin : public core::runtime::plugin
{
    /// Destructor. Do nothing.
    ~plugin() noexcept override = default;

    /// Overrides start method. Service declarations (services declared in 'service' library ).
    void start() override;

    /// Overrides stop method. Do nothing
    void stop() noexcept override;
};

} // namespace sight::module::service
