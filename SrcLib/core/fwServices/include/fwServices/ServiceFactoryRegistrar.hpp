/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2012.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __FWSERVICES_SERVICEFACTORYREGISTRAR_HPP__
#define __FWSERVICES_SERVICEFACTORYREGISTRAR_HPP__

#include "fwServices/factory/new.hpp"

#include "fwServices/registry/ServiceFactory.hpp"

namespace fwServices
{


/**
 * @brief Helper for registering a service
 * Creates internally the service factory and adds it to the FactoryRegistry
 * 
 * @date    2007-2012.
 */
template<class SRV_IMPL>
class ServiceFactoryRegistrar
{
public:

    // parameters are intentionally not references
    ServiceFactoryRegistrar(std::string simpl, std::string stype, std::string oimpl)
    {
        ::fwServices::registry::ServiceFactory::getDefault()
            ->addFactory( &::fwServices::factory::New< SRV_IMPL > , simpl, stype, oimpl);
    }

};


} //end namespace fwServices


#endif // __FWSERVICES_SERVICEFACTORYREGISTRAR_HPP__
