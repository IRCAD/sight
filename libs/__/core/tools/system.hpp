/************************************************************************
 *
 * Copyright (C) 2009-2024 IRCAD France
 * Copyright (C) 2012-2021 IHU Strasbourg
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

#include <sight/core/config.hpp>

#include <filesystem>
#include <string>

namespace sight::core::tools
{

/**
 * @brief Provide useful low-level system/os functions (pid, file move / rename).
 */
class SIGHT_CORE_CLASS_API system
{
public:

    /**
     *  @brief  Returns the pid of the current process
     */
    SIGHT_CORE_API static int get_pid() noexcept;

    /**
     * @brief   Test if process is Active
     * @return  true if the process is running
     */
    SIGHT_CORE_API static bool is_process_running(int _pid) noexcept;

    /**
     * @brief   Terminate a process
     */
    SIGHT_CORE_API static void kill_process(int _pid) noexcept;

    /**
     * @brief renames file or folder, use std::filesystem::rename first, use a copy-remove scenario if rename fails.
     * @param _from source path of the file to rename.
     * @param _to destination path of the renamed file.
     * @param _force remove the destination in all cases.
     * @throws std::filesystem_error if it fails.
     */
    SIGHT_CORE_API static void robust_rename(
        const std::filesystem::path& _from,
        const std::filesystem::path& _to,
        bool _force = false
    );
};

} // namespace sight::core::tools
