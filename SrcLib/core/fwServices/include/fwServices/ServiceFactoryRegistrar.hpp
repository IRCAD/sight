/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2016.
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
 */
template<class SRV_IMPL>
class ServiceFactoryRegistrar
{
public:

    ServiceFactoryRegistrar(const std::string& simpl, const std::string& stype)
    {
        auto factory = ::fwServices::registry::ServiceFactory::getDefault();
        factory->addServiceFactory( &::fwServices::factory::New< SRV_IMPL >, simpl, stype);
    }

    ServiceFactoryRegistrar(const std::string& simpl, const std::string& stype, const std::string& oimpl)
    {
        auto factory = ::fwServices::registry::ServiceFactory::getDefault();
        factory->addServiceFactory( &::fwServices::factory::New< SRV_IMPL >, simpl, stype);
        factory->addObjectFactory( simpl, oimpl);
    }
};

/**
 * @brief Helper for associating an object to a service implementation
 */
class ServiceObjectFactoryRegistrar
{
public:

    ServiceObjectFactoryRegistrar(const std::string& simpl, const std::string& oimpl)
    {
        auto factory = ::fwServices::registry::ServiceFactory::getDefault();
        factory->addObjectFactory( simpl, oimpl);
    }
};

} //end namespace fwServices


#endif // __FWSERVICES_SERVICEFACTORYREGISTRAR_HPP__
