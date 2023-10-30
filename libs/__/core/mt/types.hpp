/************************************************************************
 *
 * Copyright (C) 2009-2023 IRCAD France
 * Copyright (C) 2012-2015 IHU Strasbourg
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

#include <boost/thread/locks.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/thread/shared_mutex.hpp>

namespace sight::core::mt
{

using mutex       = boost::mutex;
using scoped_lock = boost::unique_lock<mutex>;

/// Defines a single writer, multiple readers mutex.
using read_write_mutex = boost::shared_mutex;
/**
 * @brief Defines a lock of read type for read/write mutex.
 * @note Multiple read lock can be done.
 */
using read_lock = boost::shared_lock<read_write_mutex>;
/**
 * @brief Defines a lock of write type for read/write mutex.
 * @note Only one write lock can be done at once.
 */
using write_lock = boost::unique_lock<read_write_mutex>;
/**
 * @brief Defines an upgradable lock type for read/write mutex.
 * @note Only one upgradable lock can be done at once but there may be multiple read lock.
 */
using read_to_write_lock = boost::upgrade_lock<read_write_mutex>;
/**
 * @brief Defines a write lock upgraded from read_to_write_lock.
 * @note Only one upgradable lock can be done at once but there may be multiple read lock.
 */
using upgrade_to_write_lock = boost::upgrade_to_unique_lock<read_write_mutex>;

} // namespace sight::core::mt
