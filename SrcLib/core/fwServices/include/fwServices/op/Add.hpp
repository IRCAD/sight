/************************************************************************
 *
 * Copyright (C) 2009-2018 IRCAD France
 * Copyright (C) 2012-2018 IHU Strasbourg
 *
 * This file is part of Sight.
 *
 * Sight is free software: you can redistribute it and/or modify it under
 * the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Sight is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with Sight. If not, see <https://www.gnu.org/licenses/>.
 *
 ***********************************************************************/

#pragma once

#include <fwTools/Object.hpp>

#include "fwServices/IService.hpp"

namespace fwServices
{

/**
 * @name Methods for creating and attaching services to object
 */
//@{
#ifndef REMOVE_DEPRECATED
/**
 * @brief Create and attach to the object obj a service of type serviceType, implementation _implementationId with the
 * universal unique identifier _id
 * @return the service
 * @note the template method is also available, where the template parameter SERVICE corresponds to the serviceType
 * @deprecated Use ::fwServices::add(srvImp, uid) instead.
 */
FWSERVICES_API ::fwServices::IService::sptr add(::fwData::Object::sptr obj, const std::string& serviceType,
                                                const std::string& _implType, const std::string& _id = "");

/**
 * @brief Create and attach to the object obj a service of type serviceType, implementation _implementationId with the
 * universal unique identifier _id.
 * @return a pointer to the new service with the given template type
 * @deprecated Use ::fwServices::add(srvImp, uid) instead.
 */
template<class SERVICE>
SPTR(SERVICE) add( ::fwData::Object::csptr obj, const std::string& _implType, const std::string& _id = "");
#endif
/**
 * @brief Create a service of type serviceType
 * @return a pointer to the new service
 */
FWSERVICES_API ::fwServices::IService::sptr add( const std::string& _implType, const std::string& _id = "" );

/**
 * @brief Create a service of type serviceType
 * @return a pointer to the new service with the given template type
 */
template<class SERVICE>
SPTR(SERVICE) add( const std::string& _implType, const std::string& _id = "" );

//@}

}

#include "fwServices/op/Add.hxx"
