/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _FWSERVICES_ADD_HPP_
#define _FWSERVICES_ADD_HPP_

#include <fwTools/Object.hpp>

#include "fwServices/ObjectServiceRegistry.hpp"
#include "fwServices/IService.hpp"

namespace fwServices
{

/**
 * @name Methods for creating and attaching services to object
 */
//@{

/**
 * @brief Create and attach to the object obj a service of type serviceType, implementation _implementationId with the universal unique identifier _id
 * @return the service
 * @note the template method is also available, where the template parameter SERVICE corresponds to the serviceType
 * @author IRCAD (Research and Development Team).
 */
FWSERVICES_API ::fwServices::IService::sptr add( ::fwTools::Object::sptr obj , std::string serviceType , std::string _implementationId , std::string _id = "") ;

template<class SERVICE>
SPTR(SERVICE) add( ::fwTools::Object::sptr obj , std::string _implementationId , std::string _id= "") ;

//@}


}

#include "fwServices/op/Add.hxx"

#endif /*_FWSERVICES_ADD_HPP_*/
