/************************************************************************
 *
 * Copyright (C) 2009-2021 IRCAD France
 * Copyright (C) 2012-2016 IHU Strasbourg
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

#pragma once

#include "service/factory/new.hpp"
#include "service/extension/Factory.hpp"

namespace sight::service
{

/**
 * @brief Helper for registering a service
 * Creates internally the service factory and adds it to the FactoryRegistry
 */
template<class SRV_IMPL>
class ServiceFactoryRegistry
{
public:

    ServiceFactoryRegistry(const std::string& simpl, const std::string& stype)
    {
        auto factory = service::extension::Factory::getDefault();
        factory->addServiceFactory( &::sight::service::factory::New< SRV_IMPL >, simpl, stype);
    }

    ServiceFactoryRegistry(const std::string& simpl, const std::string& stype, const std::string& oimpl)
    {
        auto factory = service::extension::Factory::getDefault();
        factory->addServiceFactory( &::sight::service::factory::New< SRV_IMPL >, simpl, stype);
        factory->addObjectFactory( simpl, oimpl);
    }
};

/**
 * @brief Helper for associating an object to a service implementation
 */
class ServiceObjectFactoryRegistry
{
public:

    ServiceObjectFactoryRegistry(const std::string& simpl, const std::string& oimpl)
    {
        auto factory = service::extension::Factory::getDefault();
        factory->addObjectFactory( simpl, oimpl);
    }
};

} //end namespace sight::service
