/************************************************************************
 *
 * Copyright (C) 2009-2023 IRCAD France
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

#include "service/extension/factory.hpp"
#include "service/factory/new.hpp"

namespace sight::service
{

/**
 * @brief Helper for registering a service
 * Creates internally the service factory and adds it to the FactoryRegistry
 */
template<class SRV_IMPL>
class service_factory_registry
{
public:

    service_factory_registry(const std::string& _simpl, const std::string& _stype)
    {
        auto factory = service::extension::factory::get();
        factory->add_service_factory(&sight::service::factory::make<SRV_IMPL>, _simpl, _stype);
    }

    service_factory_registry(const std::string& _simpl, const std::string& _stype, const std::string& _oimpl)
    {
        auto factory = service::extension::factory::get();
        factory->add_service_factory(&sight::service::factory::make<SRV_IMPL>, _simpl, _stype);
        factory->add_object_factory(_simpl, _oimpl);
    }
};

/**
 * @brief Helper for associating an object to a service implementation
 */
class service_object_factory_registry
{
public:

    service_object_factory_registry(const std::string& _simpl, const std::string& _oimpl)
    {
        auto factory = service::extension::factory::get();
        factory->add_object_factory(_simpl, _oimpl);
    }
};

} //end namespace sight::service
