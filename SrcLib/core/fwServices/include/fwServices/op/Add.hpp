/************************************************************************
 *
 * Copyright (C) 2009-2019 IRCAD France
 * Copyright (C) 2012-2019 IHU Strasbourg
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
