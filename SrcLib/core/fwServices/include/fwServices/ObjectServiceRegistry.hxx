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

//------------------------------------------------------------------------------

template<class SERVICE>
std::vector< SPTR(SERVICE) > ObjectServiceRegistry::getServices()
{
    std::vector< SPTR(SERVICE) >  lfwServices;
    BOOST_FOREACH( ObjectServiceRegistry::OSContainer::value_type elt, getDefault()->m_container)
    {
        SLM_ASSERT("Object expired", !elt.first.expired());
        BOOST_FOREACH(::fwServices::IService::sptr srv, elt.second)
        {
            SPTR(SERVICE) service = ::boost::dynamic_pointer_cast< SERVICE >( srv );
            if ( service )
            {
                lfwServices.push_back( service ) ;
            }
        }
    }
    SLM_DEBUG_IF("No service registered", lfwServices.empty());
    return lfwServices;
}

//------------------------------------------------------------------------------

template<class SERVICE>
std::vector< SPTR(SERVICE) > ObjectServiceRegistry::getServices( fwTools::Object::sptr obj)
{
    std::vector< SPTR(SERVICE) >  lfwServices;
    if(getDefault()->m_container.find(obj) != getDefault()->m_container.end())
    {
        std::vector< ::fwServices::IService::sptr > services = getDefault()->m_container[obj];
        BOOST_FOREACH(::fwServices::IService::sptr srv, services)
        {
            SPTR(SERVICE) service = ::boost::dynamic_pointer_cast< SERVICE >( srv );
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
std::vector< ::fwTools::Object::sptr > ObjectServiceRegistry::getObjects()
{
    std::vector< ::fwTools::Object::sptr >   lobjects;
    BOOST_FOREACH( ObjectServiceRegistry::OSContainer::value_type elt, getDefault()->m_container)
    {
        SLM_ASSERT("Object expired", !elt.first.expired());
        BOOST_FOREACH(::fwServices::IService::sptr srv, elt.second)
        {
            if(::boost::dynamic_pointer_cast< SERVICE >( srv ))
            {
                lobjects.push_back(elt.first.lock());
                break;
            }
        }
    }
    SLM_WARN_IF( "No object registered for the requested type of service", lobjects.empty() );
    return lobjects;
}

//------------------------------------------------------------------------------

template<class OBJECT,class SERVICE>
std::vector< SPTR(OBJECT) > ObjectServiceRegistry::getObjects()
{
    std::vector< SPTR(OBJECT) >   lobjects;
    BOOST_FOREACH( ObjectServiceRegistry::OSContainer::value_type elt, getDefault()->m_container)
    {
        SLM_ASSERT("Object expired", !elt.first.expired());
        SPTR(OBJECT) castObj = ::boost::dynamic_pointer_cast< OBJECT >(elt.first.lock());
        if( castObj )
        {
            BOOST_FOREACH(::fwServices::IService::sptr srv, elt.second)
            {
                if(::boost::dynamic_pointer_cast< SERVICE >( srv ))
                {
                    lobjects.push_back(castObj);
                    break;
                }
            }
        }
    }
    SLM_WARN_IF( "No object registered for the requested type of service", lobjects.empty() );
    return lobjects ;
}

//------------------------------------------------------------------------------

} // end namespace
