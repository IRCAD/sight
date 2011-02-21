/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _FWSERVICES_GET_HXX_
#define _FWSERVICES_GET_HXX_

#include <fwTools/Object.hpp>
#include <fwRuntime/ConfigurationElement.hpp>

#include "fwServices/ObjectServiceRegistry.hpp"
#include "fwServices/IService.hpp"
#include "fwServices/ComChannelService.hpp"
#include "fwServices/op/Add.hpp"

namespace fwServices
{

template<class SERVICE>
SPTR(SERVICE) get( ::fwTools::Object::sptr obj ) throw(fwTools::Failed )
{

    SPTR(SERVICE) service;
    std::vector< typename SPTR(SERVICE) > services = ::fwServices::OSR::getServices< SERVICE >( obj );

    unsigned int servicesNb = services.size();
    if(services.empty())
    {
        std::string serviceType = ::fwCore::TypeDemangler< SERVICE >().getClassname() ;
        OSLM_WARN("TODO : service "<< serviceType<< " not exist, use add to create it");

        ::boost::shared_ptr< fwServices::IService > iservice = ::fwServices::add( obj , serviceType ) ;
        assert( iservice );
        service = boost::dynamic_pointer_cast< SERVICE >( iservice ) ;
        assert( service );
    }
    else
    {
        service = *services.begin();
    }

    return service ;
}


template<class SERVICE>
SPTR(SERVICE) get( ::fwTools::Object::sptr obj, std::string uid ) throw(fwTools::Failed )
{
    std::string serviceType = ::fwCore::TypeDemangler< SERVICE >().getClassname() ;
    ::boost::shared_ptr< fwServices::IService > service = ::fwServices::get( obj , serviceType , uid ) ;
    assert( service );
    SPTR(SERVICE) castedService = boost::dynamic_pointer_cast< SERVICE >( service ) ;
    assert( castedService );
    return castedService ;
}




template<class SERVICE>
std::vector< SPTR(SERVICE) > getServices( ::fwTools::Object::sptr obj )
{
    std::string serviceType = ::fwCore::TypeDemangler< SERVICE >().getClassname() ;
    std::vector< ::boost::shared_ptr< fwServices::IService > > services = ::fwServices::getServices( obj , serviceType ) ;

    std::vector< SPTR(SERVICE) > castedServices ;

    std::vector< ::boost::shared_ptr< fwServices::IService > >::iterator iter ;
    for( iter = services.begin() ; iter != services.end() ; ++iter )
    {
        SPTR(SERVICE) castedService = boost::dynamic_pointer_cast< SERVICE >( *iter ) ;
        assert( castedService );
        castedServices.push_back( castedService ) ;
    }

    return castedServices ;
}


}

#endif /*HELPER_HPP_*/
