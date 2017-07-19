/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2015.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __DEVFORUM_TUTO04_IMAGEREADERSERVICE_HPP__
#define __DEVFORUM_TUTO04_IMAGEREADERSERVICE_HPP__

// Used to save the file system path of loaded image
#include <boost/filesystem/path.hpp>

// Include abstract class
#include <io/IReader.hpp>

// Include export definitions
#include "devForum/config.hpp"

#include "devForum/tuto03/ImageReaderService.hpp"

namespace devForum
{

namespace tuto04
{

/**
 * @brief   This service reads a vtk image.
 * @class   ImageReaderService
 */
class DEVFORUM_CLASS_API ImageReaderService : public ::devForum::tuto03::ImageReaderService
{

public:

    // Build some class definition like sptr, etc.
    fwCoreServiceClassDefinitionsMacro ( (ImageReaderService)(::io::IReader) );

    /// Constructor, does nothing.
    DEVFORUM_API ImageReaderService() noexcept;

    /// Destructor, does nothing.
    DEVFORUM_API virtual ~ImageReaderService() noexcept;

    /// This method is used to configure service with an IHM, open a dialog box to select vtk image path.
    DEVFORUM_API virtual void configureWithIHM();

};

} // end namespace tuto04

} // end namespace devForum


#endif // __DEVFORUM_TUTO04_IMAGEREADERSERVICE_HPP__
