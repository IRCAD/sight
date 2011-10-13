/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _DEVFORUM_TUTO3_IMAGEREADERSERVICE_HPP_
#define _DEVFORUM_TUTO3_IMAGEREADERSERVICE_HPP_

// Used to save the file system path of loaded image
#include <boost/filesystem/path.hpp>

// Include abstract class
#include <io/IReader.hpp>

// Include export definitions
#include "devForum/config.hpp"

#include "devForum/tuto02/ImageReaderService.hpp"

namespace devForum
{

namespace tuto03
{

/**
 * @brief   Service reads  a vtk image
 * @class   ImageReaderService.
 * @author  IRCAD (Research and Development Team).
 * @date    2009.
 */
class DEVFORUM_CLASS_API ImageReaderService : public ::devForum::tuto02::ImageReaderService
{

public :

    // Build some class definition like sptr, etc.
    fwCoreServiceClassDefinitionsMacro ( (ImageReaderService)(::io::IReader) );

    /// Constructor, does nothing.
    DEVFORUM_API ImageReaderService() throw() ;

    /// Destructor, does nothing.
    DEVFORUM_API virtual ~ImageReaderService() throw() ;


protected :

    /// This method executes the read process and notify listeners
    DEVFORUM_API virtual void updating() throw(fwTools::Failed);

    /// This method notifies other image services that a new image has been loaded.
    DEVFORUM_API virtual void notifyMessage();
};

} // end namespace tuto03

} // end namespace devForum


#endif // _DEVFORUM_TUTO2_IMAGEREADERSERVICE_HPP_
