/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009.
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
 * @class 	System
 * @author	IRCAD (Research and Development Team).
 * @date	2007-2009.
 * @todo	Complete doxygen
 */
class FWTOOLS_CLASS_API System
{
	public:

		/**
		 * @brief	Return the unique instance of the class
		 */
		FWTOOLS_API static const boost::shared_ptr< System > getDefault() throw();

		/**
		 * @brief	Return the folder where are dumped images = TempFolder / PIDOFTDVPM ...
		 */
		FWTOOLS_API boost::filesystem::path getDumpFolder() throw();


		/**
		 * @brief 	Return the temporary folder containing all DumpFolder
		 */
		FWTOOLS_API static boost::filesystem::path getTemporaryFolder() throw();

		/**
		 * @brief	Erase all files in the DumpFolder (but not itself) of the current process
		 */
		FWTOOLS_API void cleanDumpFolder() const;

		/**
		 * @brief 	Recursively erase the DumpFolder for the process given by its pid
		 */
		FWTOOLS_API void eraseDumpFolder(int pid) const;

		/**
		 * @brief 	Recursively erase the DumpFolder of zombies process
		 */
		FWTOOLS_API void eraseDumpFolderOfZombies() const;


		/**
		  * @brief 	Destructor will recursively erase dump folder if any
		  */
		virtual ~System();

		/**
		 *  @brief	Return the pid of the current Processus
		 *  @return The pid of the current Processus
		 */
		FWTOOLS_API int getPID() const;

		/**
		 * @brief	Test if process is Active
		 * @return 	true iff the process is running
		 */
		FWTOOLS_API static bool isProcessRunning(int pid);

	protected:



		System();

		/**
		 * @brief	A shared pointer to the kernel instance
		 */
		static boost::shared_ptr< System > m_instance;

		/**
		 * @brief	The path to the document's dump folder
		 */
		boost::filesystem::path m_dumpFolder;

};


} // namespace fwTools

#endif // _FWTOOLS_SYSTEM_HPP_
