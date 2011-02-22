/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <vector>
#include <boost/lexical_cast.hpp>

#include <fwTools/fwID.hpp>

#include "fwServices/Base.hpp"
#include "fwServices/IService.hpp"
#include "fwServices/op/Add.hpp"
#include "fwServices/registry/ServiceFactory.hpp"

namespace fwServices
{

//------------------------------------------------------------------------------

::fwServices::IService::sptr add( ::fwTools::Object::sptr obj , std::string serviceType , std::string _implementationId , std::string uid)
{
    ::fwServices::IService::sptr srv = ::fwServices::registry::ServiceFactory::getDefault()->create( serviceType, _implementationId );
    ::fwServices::OSR::registerService( obj , srv );
    if(!uid.empty())
    {
        OSLM_ASSERT( "Try to set ID: "<<uid<<" but already has an ID: "<<srv->getID(), !srv->hasID() );
        srv->setID( uid ) ;
    }
    return srv ;
}

//------------------------------------------------------------------------------

}

