/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2015.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __FWTOOLS_SYSTEM_HPP__
#define __FWTOOLS_SYSTEM_HPP__

#include <string>

#include <boost/filesystem/path.hpp>

#include "fwTools/config.hpp"

namespace fwTools
{

/**
 * @class   System
 * @brief Provide a system to get a Temporary folder which allow multi-user, multi-instance separation
 *
 * @date    2007-2009.
 * @todo  change "fwDumpFolder" to "localTemp"
 */
class FWTOOLS_CLASS_API System
{
public:

    /**
     * @brief   Returns the system's temporary folder.
     * Returns the value returned by boost::filesystem::temp_directory_path, or
     * if boost returns no valid dir, c:\\ on windows, /tmp on other systems
     */
    FWTOOLS_API static const ::boost::filesystem::path &getTempPath() noexcept;

    /**
     * @brief   Returns a unique per-process temporary folder.
     * The top level temporary folder will be automatically destroyed if the process ends properly
     *
     * @param   subFolderPrefix if set, creates a sub folder in temporary folder prefixed with parameter value followed
     * by a dash.
     * @return  created folder
     */
    FWTOOLS_API static const ::boost::filesystem::path getTemporaryFolder(
        const std::string& subFolderPrefix = "" ) noexcept;

    /**
     * @brief   Returns the pid of a temporary folder
     * If the given folder contains a file matching *.pid and the first part of
     * the file name is a integer, this method will return this number.
     * Otherwise, zero will be returned
     */
    FWTOOLS_API static int tempFolderPID(const ::boost::filesystem::path &dir) noexcept;

    /**
     * @brief   Clean the zombie folders of old processes in given directory
     */
    FWTOOLS_API static void cleanZombies(const ::boost::filesystem::path &dir) noexcept;

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
    static void setTempPrefix(const std::string &prefix)
    {
        s_tempPrefix = prefix;
    }

protected:

    static std::string s_tempPrefix;

};


} // namespace fwTools

#endif // __FWTOOLS_SYSTEM_HPP__
