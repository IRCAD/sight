/************************************************************************
 *
 * Copyright (C) 2009-2021 IRCAD France
 * Copyright (C) 2012-2020 IHU Strasbourg
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

#include "service/IService.hpp"

#include <core/com/Signal.hxx>

namespace sight::service
{

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------

namespace OSR
{

//------------------------------------------------------------------------------

template<class SERVICE>
std::set< SPTR(SERVICE) > getServices()
{
    return service::OSR::get()->getServices< SERVICE >();
}

//------------------------------------------------------------------------------

inline SPTR( service::registry::ObjectService::RegisterSignalType ) getRegisterSignal()
{
    return service::OSR::get()->signal< service::registry::ObjectService::RegisterSignalType >
               (service::registry::ObjectService::s_REGISTERED_SIG);
}

inline SPTR( service::registry::ObjectService::RegisterSignalType ) getUnregisterSignal()
{
    return service::OSR::get()->signal< service::registry::ObjectService::RegisterSignalType >
               (service::registry::ObjectService::s_UNREGISTERED_SIG);
}

} //namespace OSR

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------

namespace registry
{

//------------------------------------------------------------------------------

template<class SERVICE>
std::set< SPTR(SERVICE) > ObjectService::getServices() const
{
    std::set< SPTR(SERVICE) > services;

    for(const auto& srv : m_services)
    {
        SPTR(SERVICE) service = std::dynamic_pointer_cast< SERVICE >( srv );
        if ( service )
        {
            services.insert( service );
        }
    }
    return services;
}

//------------------------------------------------------------------------------

} // end registry
} // end fwServices
