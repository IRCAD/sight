/************************************************************************
 *
 * Copyright (C) 2026 IRCAD France
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

#include <sight/io/__/config.hpp>

#include <core/com/slot.hpp>
#include <core/thread/worker.hpp>

#include <data/object.hpp>
#include <data/series_set.hpp>

#include <filesystem>
#include <string>
#include <vector>

namespace sight::io::reader
{

/**
 * @brief Reads a collection of paths into a data object.
 *
 * @param _paths paths of the files or directories to read.
 * @param _data data object to populate.
 * @param _notification_slot (optional) slot receiving notifications emitted by
 *                           the selected readers.
 * @param _available_services (optional) reader services to use. If empty, all
 *                            readers matching the data object are used.
 * @param _append (optional) append loaded series to an existing series set.
 * @return true if all paths were read successfully, false otherwise.
 */

[[nodiscard]] SIGHT_IO_API bool read_paths(
    const std::vector<std::filesystem::path>& _paths,
    const sight::data::object::sptr& _data,
    const sight::core::com::slot_base::sptr& _notification_slot,
    const std::vector<std::string>& _available_services = {},
    bool _append                                        = false
);

/// Appends series that are not already present in the destination set.
/// @return The number of series skipped because they were already loaded.
SIGHT_IO_API std::size_t append_unique(
    sight::data::series_set& _destination,
    const sight::data::series_set& _source
);

} // namespace sight::io::reader
