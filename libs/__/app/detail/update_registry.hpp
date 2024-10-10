/************************************************************************
 *
 * Copyright (C) 2024 IRCAD France
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

#include <app/updater.hpp>

namespace sight::app
{

/**
 * @brief Registers updater.
 */
void register_updater(const std::string& _uid, service::base::sptr _updater);

/**
 * @brief Unregisters updater.
 */
void unregister_updater(const std::string& _uid);

/**
 * @brief Returns updater matching the UID, null if  not found.
 */
service::base::sptr get_updater(const std::string& _uid);

} // namespace sight::app
