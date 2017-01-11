/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __FWSERVICES_OP_ADD_HPP__
#define __FWSERVICES_OP_ADD_HPP__

#include <fwTools/Object.hpp>

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
 *
 */
FWSERVICES_API ::fwServices::IService::sptr add( ::fwData::Object::sptr obj, std::string serviceType,
                                                 std::string _implementationId, std::string _id = "");

template<class SERVICE>
SPTR(SERVICE) add( ::fwData::Object::csptr obj, std::string _implementationId, std::string _id = "");

//@}


}

#include "fwServices/op/Add.hxx"

#endif /*__FWSERVICES_OP_ADD_HPP__*/
