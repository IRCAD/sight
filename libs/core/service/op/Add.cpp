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

#include "service/op/Add.hpp"

#include "service/IService.hpp"
#include "service/macros.hpp"
#include "service/registry/ServiceFactory.hpp"

#include <core/tools/fwID.hpp>

#include <boost/lexical_cast.hpp>

#include <vector>

namespace sight::service
{

//------------------------------------------------------------------------------

service::IService::sptr add( const std::string& _implType, const std::string& _uid)
{
    service::IService::sptr srv;
    srv = service::registry::ServiceFactory::getDefault()->create( _implType );
    service::OSR::registerService( srv );
    FW_RAISE_IF("Failed to add " + _implType, !srv );
    if(!_uid.empty())
    {
        SLM_ASSERT( "Try to set ID: " + _uid + " but already has an ID: " + srv->getID(), !srv->hasID() );
        srv->setID( _uid );
    }
    return srv;
}

//------------------------------------------------------------------------------

}
