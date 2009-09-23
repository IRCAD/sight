/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as 
 * published by the Free Software Foundation.  
 * ****** END LICENSE BLOCK ****** */

#ifndef _FWSERVICES_DELETER_HPP_
#define _FWSERVICES_DELETER_HPP_

#include <fwTools/Object.hpp>
#include "fwServices/config.hpp"

namespace fwServices
{

/**
 * @class DefaultObjectDeleter
 * @brief Intercept ::fwTools::Object deletion to ensure that all associated service (including communication) are stopped and unregistered
 *
 * @author IRCAD (Research and Development Team).
 * @author Jean-Baptiste Fasquel
 */
struct FWSERVICES_CLASS_API DefaultObjectDeleter : public ::fwTools::IDeleter
{
	/**
	 * @brief Constructor
	 */
	FWSERVICES_API DefaultObjectDeleter();
	/**
	 * @brief Destructor
	 */
	FWSERVICES_API virtual ~DefaultObjectDeleter();

	/**
	 * @brief Invoked from obj destructor and trigger service unregistration
	 */
	FWSERVICES_API void Delete(::fwTools::Object *obj) ;
} ;


}

#endif /*_FWSERVICES_ADD_HPP_*/
