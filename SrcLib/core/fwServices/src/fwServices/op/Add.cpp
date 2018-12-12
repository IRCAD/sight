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

#include "fwServices/op/Add.hpp"

#include "fwServices/IService.hpp"
#include "fwServices/macros.hpp"
#include "fwServices/registry/ServiceFactory.hpp"

#include <fwTools/fwID.hpp>

#include <boost/lexical_cast.hpp>

#include <vector>

namespace fwServices
{

//------------------------------------------------------------------------------
#ifndef REMOVE_DEPRECATED
::fwServices::IService::sptr add( ::fwData::Object::sptr _obj,
                                  const std::string& _serviceType,
                                  const std::string& _implType,
                                  const std::string& _uid)
{
    FW_DEPRECATED_MSG("'fwServices::add(object, srvType, srvImpl[,uid])' is deprecated.", "20.0");
    ::fwServices::IService::sptr srv;
    srv = ::fwServices::registry::ServiceFactory::getDefault()->create( _serviceType, _implType );
    FW_RAISE_IF("Failed to add " + _implType, !srv );
    ::fwServices::OSR::registerService( _obj, srv );
    if(!_uid.empty())
    {
        SLM_ASSERT( "Try to set ID: " + _uid + " but already has an ID: " + srv->getID(), !srv->hasID() );
        srv->setID( _uid );
    }
    return srv;
}
#endif
//------------------------------------------------------------------------------

::fwServices::IService::sptr add( const std::string& _implType, const std::string& _uid)
{
    ::fwServices::IService::sptr srv;
    srv = ::fwServices::registry::ServiceFactory::getDefault()->create( _implType );
    ::fwServices::OSR::registerService( srv );
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
