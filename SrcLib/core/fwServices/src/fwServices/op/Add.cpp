/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2018.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

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
