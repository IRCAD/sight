/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as 
 * published by the Free Software Foundation.  
 * ****** END LICENSE BLOCK ****** */

#ifndef _IO_IWRITER_HPP_
#define _IO_IWRITER_HPP_

#include <fwServices/IService.hpp>

#include "io/config.hpp"

namespace io
{

/**
 * @brief Writer service API. It manages extension points definition and extension configuration
 * @class	IWriter.
 * @author	IRCAD (Research and Development Team).
 * @date	2007-2009.
 *
 * This class represents the base interface for writer services.
 * Use the base service methods :
 * @li The service is configured with methods setConfiguration(cfg) and configure()
 * @li The method start() initialize the service
 * @li To write the object use update() method
 * @li Finally we must call stop() before deleting the service
 */
class IO_CLASS_API IWriter : public fwServices::IService
{

public :
    fwCoreServiceClassDefinitionsMacro ( (IWriter)(::fwServices::IService::Baseclass) ) ;

	/**
	 * @brief	Constructor (by default does nothing)
	 */
	IO_API IWriter() throw() ;

	/**
	 * @brief	Destructor
	 */
	IO_API virtual ~IWriter() throw() ;

	/**
	 * @name	Specific service methods for writing
	 */
	//@{

	/**
	 * @brief Configure the image path (by default does nothing).
	 *
	 * This method is used to find
	 * the file path  using a file selector.
	 */
	IO_API virtual void configureWithIHM() = 0 ;
	///@}

};

}


#endif // _IO_IWRITER_HPP_
