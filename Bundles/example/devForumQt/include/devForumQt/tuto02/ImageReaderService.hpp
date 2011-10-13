/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _DEVFORUMQT_TUTO2_IMAGEREADERSERVICE_HPP_
#define _DEVFORUMQT_TUTO2_IMAGEREADERSERVICE_HPP_

// Used to save the file system path of loaded image
#include <boost/filesystem/path.hpp>

// Include abstract class
#include <io/IReader.hpp>

// Include export definitions
#include "devForumQt/config.hpp"

namespace devForum
{

namespace tuto02
{

/**
 * @brief   Service reads  a vtk image
 * @class   ImageReaderService.
 * @author  IRCAD (Research and Development Team).
 * @date    2009.
 */
class DEVFORUMQT_CLASS_API ImageReaderService : public ::io::IReader
{

public :

    // Build some class definition like sptr, etc.
    fwCoreServiceClassDefinitionsMacro ( (ImageReaderService)(::io::IReader) );

    /// Constructor, does nothing.
    DEVFORUMQT_API ImageReaderService() throw() ;

    /// Destructor, does nothing.
    DEVFORUMQT_API virtual ~ImageReaderService() throw() ;


protected :

    /// This method is called when the service start, here does nothing.
    DEVFORUMQT_API virtual void starting() throw(fwTools::Failed);

    /// This method is called when the service start, here does nothing.
    DEVFORUMQT_API virtual void stopping() throw(fwTools::Failed);

    /**
     * @brief Configure the reader service.
     *
     * This method is used to configure the service. It search in a configure element
     * the definition of image path. XML configuration sample:
     */
    DEVFORUMQT_API virtual void configuring() throw(::fwTools::Failed);

    /// This method is used to configure service with an IHM, here does nothing.
    DEVFORUMQT_API virtual void configureWithIHM();

    /// This method executes the read process.
    DEVFORUMQT_API virtual void updating() throw(fwTools::Failed);

    /// This method is called on msg notification, here does nothing
    DEVFORUMQT_API virtual void updating( ::fwServices::ObjectMsg::csptr _msg ) throw(::fwTools::Failed);

    /// Image path, location of image on filesystem.
    ::boost::filesystem::path m_fsImgPath;
};

} // end namespace tuto02

} // end namespace devForum


#endif // _DEVFORUMQT_TUTO2_IMAGEREADERSERVICE_HPP_
