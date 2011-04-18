/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _IO_IREADER_HPP_
#define _IO_IREADER_HPP_

#include <fwServices/IService.hpp>

#include "io/config.hpp"

namespace io
{

/**
 * @brief Reader service API. It manages extension points definition and extension configuration
 * @class   IReader.
 * @author  IRCAD (Research and Development Team).
 * @date    2007-2009.
 *
 * This class represents the base interface for reader services.
 * Use the base service methods :
 * @li The service is configured with methods setConfiguration(cfg) and configure()
 * @li The method start() initialize the service
 * @li To read the object use update() method
 * @li Finally we must call stop() before deleting the service
 * @todo ACH : remove some methods : getSupportedExtensions ? getSelectorDialogTitle ? setLocation/getLocation ? location can be replaced perhaps by ::fwData::Location ( and not a ::boost::path )
 */
class IO_CLASS_API IReader : public fwServices::IService
{

public :
    fwCoreServiceClassDefinitionsMacro ( (IReader)(::fwServices::IService) ) ;

    /**
     * @name    Specific service methods for reading
     */
    //@{

    /**
     * @brief Configure the image path (by default does nothing).
     *
     * This method is used to find
     * the file path  using a file selector.
     */
    IO_API virtual void configureWithIHM() = 0;

    /**
     * @brief   returns  (filename) extension
     */
    IO_API virtual std::vector< std::string > getSupportedExtensions() ;

    /**
     * @brief   returns  the title of selector dialog box
     */
    IO_API virtual std::string getSelectorDialogTitle();
    //@}

protected:

    /**
     * @brief   Constructor (by default does nothing)
     */
    IO_API IReader() throw() ;

    /**
     * @brief   Destructor
     */
    IO_API virtual ~IReader() throw() ;

};

}


#endif /*_IO_IREADER_HPP_*/
