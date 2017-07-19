/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2015.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __DEVFORUM_TUTO02_IMAGEREADERSERVICE_HPP__
#define __DEVFORUM_TUTO02_IMAGEREADERSERVICE_HPP__

// Used to save the file system path of loaded image
#include <boost/filesystem/path.hpp>

// Include abstract class
#include <io/IReader.hpp>

// Include export definitions
#include "devForum/config.hpp"

namespace devForum
{

namespace tuto02
{

/**
 * @brief   This service reads a vtk image.
 * @class   ImageReaderService
 */
class DEVFORUM_CLASS_API ImageReaderService : public ::io::IReader
{

public:

    // Build some class definition like sptr, etc.
    fwCoreServiceClassDefinitionsMacro ( (ImageReaderService)(::io::IReader) );

    /// Constructor, does nothing.
    DEVFORUM_API ImageReaderService() noexcept;

    /// Destructor, does nothing.
    DEVFORUM_API virtual ~ImageReaderService() noexcept;


protected:

    /// This method is called when the service start, here does nothing.
    DEVFORUM_API virtual void starting();

    /// This method is called when the service start, here does nothing.
    DEVFORUM_API virtual void stopping();

    /**
     * @brief Configure the reader service.
     *
     * This method is used to configure the service. It search in a configure element
     * the definition of image path. XML configuration sample:
     */
    DEVFORUM_API virtual void configuring();

    /// This method is used to configure service with an IHM, here does nothing.
    DEVFORUM_API virtual void configureWithIHM();

    /// This method executes the read process.
    DEVFORUM_API virtual void updating();

    /// Image path, location of image on filesystem.
    ::boost::filesystem::path m_fsImgPath;
};

} // end namespace tuto02

} // end namespace devForum


#endif // __DEVFORUM_TUTO02_IMAGEREADERSERVICE_HPP__
