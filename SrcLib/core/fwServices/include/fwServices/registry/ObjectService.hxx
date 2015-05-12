/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2015.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __FWSERVICES_REGISTRY_OBJECTSERVICE_HXX__
#define __FWSERVICES_REGISTRY_OBJECTSERVICE_HXX__

#include <boost/foreach.hpp>

#include <fwCore/base.hpp>

#include <fwServices/IService.hpp>

namespace fwServices
{

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------

namespace OSR
{

template<class SERVICE>
std::vector< SPTR(SERVICE) > getServices()
{
    return ::fwServices::OSR::get()->getServices< SERVICE >();
}

template<class SERVICE>
std::vector< SPTR(SERVICE) > getServices(::fwData::Object::sptr obj)
{
    return ::fwServices::OSR::get()->getServices< SERVICE >(obj);
}

} //namespace OSR

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------

namespace registry
{


//------------------------------------------------------------------------------

template<class SERVICE>
std::vector< SPTR(SERVICE) > ObjectService::getServices() const
{
    std::vector< SPTR(SERVICE) > services;
    const ServiceContainerType::right_map &right = m_container.right;
    BOOST_FOREACH( const ServiceContainerType::right_map::value_type &elt, right)
    {
        SPTR(SERVICE) service = ::boost::dynamic_pointer_cast< SERVICE >( elt.first );
        if ( service )
        {
            services.push_back( service );
        }
    }
    SLM_DEBUG_IF("No service registered", services.empty());
    return services;
}

//------------------------------------------------------------------------------

template<class SERVICE>
std::vector< SPTR(SERVICE) > ObjectService::getServices(::fwData::Object::sptr obj) const
{
    std::vector< SPTR(SERVICE) > services;
    if(m_container.left.find(obj->getOSRKey()->getLogicStamp()) != m_container.left.end())
    {
        ServiceContainerType::left_map::const_iterator iter;
        ::fwCore::LogicStamp::LogicStampType key = obj->getOSRKey()->getLogicStamp();
        ServiceContainerType::left_map::const_iterator firstElement = m_container.left.find(key);
        ServiceContainerType::left_map::const_iterator lastElement  = m_container.left.upper_bound(key);
        for (iter = firstElement; iter != lastElement; ++iter)
        {
            SPTR(SERVICE) service = ::boost::dynamic_pointer_cast< SERVICE >( iter->second );
            if ( service)
            {
                services.push_back( service );
            }
        }
    }
    return services;
}

//------------------------------------------------------------------------------

template<class SERVICE>
ObjectService::ObjectVectorType ObjectService::getObjects() const
{
    ObjectVectorType objects;
    const ServiceContainerType::right_map & right = m_container.right;
    BOOST_FOREACH( const ServiceContainerType::right_map::value_type &elt, right)
    {
        SPTR(SERVICE) service = ::boost::dynamic_pointer_cast< SERVICE >( elt.first );
        if ( service && std::find(objects.begin(), objects.end(), service->getObject()) == objects.end() )
        {
            objects.push_back( service->getObject() );
        }
    }
    SLM_WARN_IF( "No object registered for the requested type of service", objects.empty() );
    return objects;
}

//------------------------------------------------------------------------------

} // end registry
} // end fwServices

#endif // __FWSERVICES_REGISTRY_OBJECTSERVICE_HXX__
