/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2012.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _FWTOOLS_SYSTEM_HPP_
#define _FWTOOLS_SYSTEM_HPP_

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
    FWTOOLS_API static const ::boost::filesystem::path &getTempPath() throw();

    /**
     * @brief   Returns a unique per-process temporary folder.
     * The returned folder will be automatically destroyed if the process ends
     * properly
     */
    FWTOOLS_API static const ::boost::filesystem::path &getTemporaryFolder() throw();

    /**
     * @brief   Returns the pid of a temporary folder
     * If the given folder contains a file matching *.pid and the first part of
     * the file name is a integer, this method will return this number.
     * Otherwise, zero will be returned
     */
    FWTOOLS_API static int tempFolderPID(const ::boost::filesystem::path &dir) throw();

    /**
     * @brief   Clean the zombie folders of old processes in given directory
     */
    FWTOOLS_API static void cleanZombies(const ::boost::filesystem::path &dir) throw();

    /**
     *  @brief  Returns the pid of the current process
     */
    FWTOOLS_API static int getPID() throw();

    /**
     * @brief   Test if process is Active
     * @return  true iff the process is running
     */
    FWTOOLS_API static bool isProcessRunning(int pid) throw();

    /**
     * @brief Sets the temporary folder prefix.
     * The prefix must be set before the first call to getTemporaryFolder,
     * otherwise, it won't be used.
     */
    static void setTempPrefix(const std::string &prefix)
    {
        s_tempPrefix = prefix;
    };

protected:

    static std::string s_tempPrefix;

};


} // namespace fwTools

#endif // _FWTOOLS_SYSTEM_HPP_
