/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _FWSERVICES_GET_HPP_
#define _FWSERVICES_GET_HPP_

#include <fwTools/Object.hpp>

#include "fwServices/ObjectServiceRegistry.hpp"
#include "fwServices/IService.hpp"

namespace fwServices
{

/**
 * @name Methods for accessing services
 */
//@{

/**
 * @brief Get the first service of type serviceType which is attached to obj
 * @note The template method is also available, where the template parameter SERVICE relates the service type serviceType
 * @todo Do not automatically add service, raise an error instead
 * @return the requested service, null if not
 * @author IRCAD (Research and Development Team).
 */
FWSERVICES_API ::fwServices::IService::sptr get( ::fwTools::Object::sptr obj, std::string serviceType ) throw(fwTools::Failed );

template<class SERVICE>
SPTR(SERVICE) get( ::fwTools::Object::sptr obj) throw(fwTools::Failed );

/**
 * @brief Return a registered IService having uid as unique universal identifier , its an alias on fwTools::Object::getID(...) method
 * @author IRCAD (Research and Development Team).

 */
FWSERVICES_API ::fwServices::IService::sptr get( std::string uid ) throw(fwTools::Failed );


//@}


}

#include "fwServices/op/Get.hxx"

#endif /*_FWSERVICES_GET_HPP_*/
