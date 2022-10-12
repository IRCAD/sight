/************************************************************************
 *
 * Copyright (C) 2009-2022 IRCAD France
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

#include "core/config.hpp"

#include <filesystem>
#include <string>

namespace sight::core::tools
{

/**
 * @brief Provide a system to get a Temporary folder which allow multi-user, multi-instance separation
 *
 * @todo  change "fwDumpFolder" to "localTemp"
 */
class CORE_CLASS_API System
{
public:

    /// Convenience class that deletes the associated temporary file on destruction
    class TemporaryFile
    {
    public:

        TemporaryFile(const TemporaryFile&)            = delete;
        TemporaryFile(TemporaryFile&&)                 = delete;
        TemporaryFile& operator=(const TemporaryFile&) = delete;
        TemporaryFile& operator=(TemporaryFile&&)      = delete;

        inline TemporaryFile() :
            m_filePath(getTemporaryFolder() / genTempFileName())
        {
        }

        inline ~TemporaryFile()
        {
            std::filesystem::remove(m_filePath);
        }

        //------------------------------------------------------------------------------

        [[nodiscard]] inline const std::filesystem::path& filePath() const
        {
            return m_filePath;
        }

    private:

        const std::filesystem::path m_filePath;
    };

    /**
     * @brief   Returns the system's temporary folder.
     * Returns the value returned by std::filesystem::temp_directory_path, or
     * if std returns no valid dir, c:\\ on windows, /tmp on other systems
     */
    CORE_API static const std::filesystem::path& getTempPath() noexcept;

    /**
     * @brief   Generate a random filename.
     * @param   length: the length of the generated filename.
     */
    CORE_API static std::string genTempFileName(std::size_t _length = 64);

    /**
     * @brief   Returns a unique per-process temporary folder.
     * The top level temporary folder will be automatically destroyed if the process ends properly
     *
     * @param   subFolderPrefix if set, creates a sub folder in temporary folder prefixed with parameter value followed
     * by a dash.
     * @return  created folder
     */
    CORE_API static std::filesystem::path getTemporaryFolder(
        const std::string& subFolderPrefix = ""
    ) noexcept;

    /**
     * @brief   Returns the pid of a temporary folder
     * If the given folder contains a file matching *.pid and the first part of
     * the file name is a integer, this method will return this number.
     * Otherwise, zero will be returned
     */
    CORE_API static int tempFolderPID(const std::filesystem::path& dir) noexcept;

    /**
     * @brief   Clean the zombie folders of old processes in given directory
     */
    CORE_API static void cleanAllTempFolders(const std::filesystem::path& dir) noexcept;

    /**
     *  @brief  Returns the pid of the current process
     */
    CORE_API static int getPID() noexcept;

    /**
     * @brief   Test if process is Active
     * @return  true if the process is running
     */
    CORE_API static bool isProcessRunning(int pid) noexcept;

    /**
     * @brief renames file or folder, use std::filesystem::rename first, use a copy-remove scenario if rename fails.
     * @param _from source path of the file to rename.
     * @param _to destination path of the renamed file.
     * @param _force remove the destination in all cases.
     * @throws std::filesystem_error if it fails.
     */
    CORE_API static void robustRename(
        const std::filesystem::path& _from,
        const std::filesystem::path& _to,
        bool _force = false
    );

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

} // namespace sight::core::tools
