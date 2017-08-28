/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __FWSERVICES_REGISTRY_OBJECTSERVICE_HXX__
#define __FWSERVICES_REGISTRY_OBJECTSERVICE_HXX__

#include <fwServices/IService.hpp>

#include <fwCom/Signal.hxx>

namespace fwServices
{

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------

namespace OSR
{

//------------------------------------------------------------------------------

template<class SERVICE>
std::set< SPTR(SERVICE) > getServices()
{
    return ::fwServices::OSR::get()->getServices< SERVICE >();
}

//------------------------------------------------------------------------------

template<class SERVICE>
std::set< SPTR(SERVICE) > getServices(::fwData::Object::sptr obj)
{
    return ::fwServices::OSR::get()->getServices< SERVICE >(obj);
}

inline SPTR( ::fwServices::registry::ObjectService::RegisterSignalType ) getRegisterSignal()
{
    return ::fwServices::OSR::get()->signal< ::fwServices::registry::ObjectService::RegisterSignalType >
               (::fwServices::registry::ObjectService::s_REGISTERED_SIG);
}

inline SPTR( ::fwServices::registry::ObjectService::RegisterSignalType ) getUnregisterSignal()
{
    return ::fwServices::OSR::get()->signal< ::fwServices::registry::ObjectService::RegisterSignalType >
               (::fwServices::registry::ObjectService::s_UNREGISTERED_SIG);
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
    const ServiceContainerType::right_map& right = m_container.right;
    for( const ServiceContainerType::right_map::value_type& elt: right)
    {
        SPTR(SERVICE) service = std::dynamic_pointer_cast< SERVICE >( elt.first );
        if ( service )
        {
            services.insert( service );
        }
    }
    SLM_DEBUG_IF("No service registered", services.empty());
    return services;
}

//------------------------------------------------------------------------------

template<class SERVICE>
std::set< SPTR(SERVICE) > ObjectService::getServices(::fwData::Object::sptr obj) const
{
    std::set< SPTR(SERVICE) > services;
    if(m_container.left.find(obj) != m_container.left.end())
    {
        ServiceContainerType::left_map::const_iterator iter;
        ServiceContainerType::left_map::const_iterator firstElement = m_container.left.lower_bound(obj);
        ServiceContainerType::left_map::const_iterator lastElement  = m_container.left.upper_bound(obj);
        for (iter = firstElement; iter != lastElement; ++iter)
        {
            SPTR(SERVICE) service = std::dynamic_pointer_cast< SERVICE >( iter->second );
            if ( service)
            {
                services.insert( service );
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
    const ServiceContainerType::right_map& right = m_container.right;
    for( const ServiceContainerType::right_map::value_type& elt : right)
    {
        SPTR(SERVICE) service = std::dynamic_pointer_cast< SERVICE >( elt.first );
        if ( service && std::find(objects.begin(), objects.end(), service->getObject()) == objects.end() )
        {
            objects.insert( service->getObject() );
        }
    }
    SLM_WARN_IF( "No object registered for the requested type of service", objects.empty() );
    return objects;
}

//------------------------------------------------------------------------------

} // end registry
} // end fwServices

#endif // __FWSERVICES_REGISTRY_OBJECTSERVICE_HXX__
