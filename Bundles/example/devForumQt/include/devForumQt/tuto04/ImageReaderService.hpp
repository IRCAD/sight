/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _DEVFORUMQT_TUTO4_IMAGEREADERSERVICE_HPP_
#define _DEVFORUMQT_TUTO4_IMAGEREADERSERVICE_HPP_

// Used to save the file system path of loaded image
#include <boost/filesystem/path.hpp>

// Include abstract class
#include <io/IReader.hpp>

// Include export definitions
#include "devForumQt/config.hpp"

#include "devForumQt/tuto03/ImageReaderService.hpp"

namespace devForum
{

namespace tuto04
{

/**
 * @brief   Service reads  a vtk image
 * @class   ImageReaderService.
 * @author  IRCAD (Research and Development Team).
 * @date    2009.
 */
class DEVFORUMQT_CLASS_API ImageReaderService : public ::devForum::tuto03::ImageReaderService
{

public :

    // Build some class definition like sptr, etc.
    fwCoreServiceClassDefinitionsMacro ( (ImageReaderService)(::io::IReader) );

    /// Constructor, does nothing.
    DEVFORUMQT_API ImageReaderService() throw() ;

    /// Destructor, does nothing.
    DEVFORUMQT_API virtual ~ImageReaderService() throw() ;

    /// This method is used to configure service with an IHM, open a dialog box to select vtk image path.
    DEVFORUMQT_API virtual void configureWithIHM();

};

} // end namespace tuto04

} // end namespace devForum


#endif // _DEVFORUMQT_TUTO4_IMAGEREADERSERVICE_HPP_
