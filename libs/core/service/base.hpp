/************************************************************************
 *
 * Copyright (C) 2021-2023 IRCAD France
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

#include <service/macros.hpp>
#include <service/op/Add.hpp>
#include <service/op/Get.hpp>
#include <service/registry.hpp>

namespace sight::service
{

/**
 * @brief Remove an existing service.
 * This provides a symmetric function to sight::service::add().
 * @return a pointer to the new service
 */
inline void remove(const SPTR(service::IService)& _srv)
{
    service::unregisterService(_srv);
}

//------------------------------------------------------------------------------

} // namespace sight::service
