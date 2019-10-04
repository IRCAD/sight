/************************************************************************
 *
 * Copyright (C) 2009-2019 IRCAD France
 * Copyright (C) 2012-2019 IHU Strasbourg
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

#include "fwTools/config.hpp"

#include <filesystem>

#include <string>

namespace fwTools
{

/**
 * @brief Provide a system to get a Temporary folder which allow multi-user, multi-instance separation
 *
 * @todo  change "fwDumpFolder" to "localTemp"
 */
class FWTOOLS_CLASS_API System
{
public:

    /**
     * @brief   Returns the system's temporary folder.
     * Returns the value returned by std::filesystem::temp_directory_path, or
     * if boost returns no valid dir, c:\\ on windows, /tmp on other systems
     */
    FWTOOLS_API static const std::filesystem::path& getTempPath() noexcept;

    /**
     * @brief   Returns a unique per-process temporary folder.
     * The top level temporary folder will be automatically destroyed if the process ends properly
     *
     * @param   subFolderPrefix if set, creates a sub folder in temporary folder prefixed with parameter value followed
     * by a dash.
     * @return  created folder
     */
    FWTOOLS_API static const std::filesystem::path getTemporaryFolder(
        const std::string& subFolderPrefix = "" ) noexcept;

    /**
     * @brief   Returns the pid of a temporary folder
     * If the given folder contains a file matching *.pid and the first part of
     * the file name is a integer, this method will return this number.
     * Otherwise, zero will be returned
     */
    FWTOOLS_API static int tempFolderPID(const std::filesystem::path& dir) noexcept;

    /**
     * @brief   Clean the zombie folders of old processes in given directory
     */
    FWTOOLS_API static void cleanAllTempFolders(const std::filesystem::path& dir) noexcept;

    /**
     *  @brief  Returns the pid of the current process
     */
    FWTOOLS_API static int getPID() noexcept;

    /**
     * @brief   Test if process is Active
     * @return  true if the process is running
     */
    FWTOOLS_API static bool isProcessRunning(int pid) noexcept;

    /**
     * @brief Sets the temporary folder prefix.
     * The prefix must be set before the first call to getTemporaryFolder,
     * otherwise, it won't be used.
     */
    static void setTempPrefix(const std::string& prefix)
    {
        s_tempPrefix = prefix;
    }

protected:

    static std::string s_tempPrefix;

};

} // namespace fwTools
