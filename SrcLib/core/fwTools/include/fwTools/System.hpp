/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _FWTOOLS_SYSTEM_HPP_
#define _FWTOOLS_SYSTEM_HPP_

#include <exception>
#include <string>
#include <boost/filesystem/path.hpp>

#include "fwTools/config.hpp"

namespace fwTools
{

/**
 * @class       System
 * @brief Provide a system to get a Temporary folder which allow multi-user, multi-instance separation
 * @author      IRCAD (Research and Development Team).
 * @date        2007-2009.
 * @todo  change "fwDumpFolder" to "localTemp"
 */
class FWTOOLS_CLASS_API System
{
        public:

                /**
                 * @brief       Return the unique instance of the class
                 */
                FWTOOLS_API static const boost::shared_ptr< System > getDefault() throw();

                /**
                 * @brief       Return the folder where are dumped data = TempFolder / PIDOFTPROCESS ...
                 * The Dump folder change to each new instance of the application
                 * @return path equal to getTemporaryFolder()/getPID()
                 */
                FWTOOLS_API boost::filesystem::path getDumpFolder() throw();


                /**
                 * @brief       Return <b>the temporary folder</b> containing all DumpFolder
                 * The Folder used look like <tt>ROOT/fwDumpFolder-USERNAME</tt> directory. The USERNAME is get from our
                 * variable environement
                 * \li Under Linux ROOT is <tt>/tmp</tt> directory.
                 * \li Under Windows ROOT is obtain form \%TMP% or \%TEMP% env. if they do not exist then it will try ROOT=D:\\ then ROOT=C:\\
                 */
                FWTOOLS_API static boost::filesystem::path getTemporaryFolder() throw();

                /**
                 * @brief       Erase all files in the DumpFolder (but not itself) of the current process
                 */
                FWTOOLS_API void cleanDumpFolder() const;

                /**
                 * @brief       Recursively erase the DumpFolder for the process given by its pid
                 * @note no existing pid are ignored
                 */
                FWTOOLS_API void eraseDumpFolder(int pid) const;

                /**
                 * @brief       Recursively erase the DumpFolder of zombies process
                 */
                FWTOOLS_API void eraseDumpFolderOfZombies() const;


                /**
                  * @brief      Destructor will recursively erase dump folder if any
                  */
                virtual ~System();

                /**
                 *  @brief      Return the pid of the current Processus
                 *  @return The pid of the current Processus
                 */
                FWTOOLS_API int getPID() const;

                /**
                 * @brief       Test if process is Active
                 * @return      true iff the process is running
                 */
                FWTOOLS_API static bool isProcessRunning(int pid);

        protected:



                System();

                /**
                 * @brief       A shared pointer to the kernel instance
                 */
                static boost::shared_ptr< System > m_instance;

                /**
                 * @brief       The path to the document's dump folder
                 */
                boost::filesystem::path m_dumpFolder;

};


} // namespace fwTools

#endif // _FWTOOLS_SYSTEM_HPP_
