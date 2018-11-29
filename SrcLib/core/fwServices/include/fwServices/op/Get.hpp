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
 * @name Methods for accessing services
 */
//@{
#ifndef REMOVE_DEPRECATED
/**
 * @brief Get the first service of type serviceType which is attached to obj
 * @note The template method is also available, where the template parameter SERVICE relates the service type
 * serviceType
 * @todo Do not automatically add service, raise an error instead
 * @return the requested service, null if not
 * @deprecated It will be removed in 20.0
 */
FWSERVICES_API ::fwServices::IService::sptr get( ::fwData::Object::sptr obj,
                                                 std::string serviceType );

/**
 * @deprecated It will be removed in 20.0
 */
template<class SERVICE>
SPTR(SERVICE) get( ::fwData::Object::sptr obj);
#endif
/**
 * @brief Return a registered IService having uid as unique universal identifier , its an alias on
 * fwData::Object::getID(...) method
 *

 */
FWSERVICES_API ::fwServices::IService::sptr get( std::string uid );

//@}

}

#include "fwServices/op/Get.hxx"
