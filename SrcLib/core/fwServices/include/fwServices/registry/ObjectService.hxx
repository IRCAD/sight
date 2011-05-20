/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <iostream>
#include <sstream>
#include <iterator>

#include <boost/foreach.hpp>

#include <fwCore/base.hpp>
#include <fwTools/ClassFactoryRegistry.hpp>

#include <fwServices/IService.hpp>

namespace fwServices
{
namespace registry
{
//------------------------------------------------------------------------------

template<class SERVICE>
std::vector< SPTR(SERVICE) > ObjectService::getServices()
{
    std::vector< SPTR(SERVICE) >  lfwServices;
    ObjectService::KSContainer::right_map & right = getDefault()->m_container.right;
    BOOST_FOREACH( ObjectService::KSContainer::right_map::value_type elt, right)
    {
        SPTR(SERVICE) service = ::boost::dynamic_pointer_cast< SERVICE >( elt.first );
        if ( service )
        {
            lfwServices.push_back( service ) ;
        }
    }
    SLM_DEBUG_IF("No service registered", lfwServices.empty());
    return lfwServices;
}

//------------------------------------------------------------------------------

template<class SERVICE>
std::vector< SPTR(SERVICE) > ObjectService::getServices( ::fwTools::Object::sptr obj)
{
    std::vector< SPTR(SERVICE) >  lfwServices;
    if(getDefault()->m_container.left.find(obj->getOSRKey()->getLogicStamp()) != getDefault()->m_container.left.end())
    {
        ObjectService::KSContainer::left_map::iterator iter;
        ::fwCore::LogicStamp::LogicStampType key = obj->getOSRKey()->getLogicStamp();
        ObjectService::KSContainer::left_map::iterator firstElement = getDefault()->m_container.left.find(key);
        ObjectService::KSContainer::left_map::iterator lastElement = getDefault()->m_container.left.upper_bound(key);
        for (iter = firstElement ; iter != lastElement ; ++iter)
        {
            SPTR(SERVICE) service = ::boost::dynamic_pointer_cast< SERVICE >( iter->second );
            if ( service)
            {
                lfwServices.push_back( service ) ;
            }
        }
    }
    return lfwServices;
}

//------------------------------------------------------------------------------

template<class SERVICE>
std::vector< ::fwTools::Object::sptr > ObjectService::getObjects()
{
    std::vector< ::fwTools::Object::sptr >   lobjects;
    ObjectService::KSContainer::right_map & right = getDefault()->m_container.right;
    BOOST_FOREACH( ObjectService::KSContainer::right_map::value_type elt, right)
    {
        SPTR(SERVICE) service = ::boost::dynamic_pointer_cast< SERVICE >( elt.first );
        if ( service && std::find(lobjects.begin(), lobjects.end(), service->getObject()) == lobjects.end() )
        {
            lobjects.push_back( service->getObject() ) ;
        }
    }
    SLM_WARN_IF( "No object registered for the requested type of service", lobjects.empty() );
    return lobjects;
}

//------------------------------------------------------------------------------

} // end registry
} // end fwServices
