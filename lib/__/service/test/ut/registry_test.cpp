/************************************************************************
 *
 * Copyright (C) 2009-2026 IRCAD France
 * Copyright (C) 2012-2020 IHU Strasbourg
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

#include "test_service.hpp"

#include <data/integer.hpp>
#include <data/real.hpp>

#include <service/base.hpp>
#include <service/extension/factory.hpp>
#include <service/op.hpp>
#include <service/registry.hpp>

#include <doctest/doctest.h>

TEST_SUITE("sight::service::registry")
{
    TEST_CASE("register_key")
    {
        const std::string srv_type("sight::service::ut::test_srv");
        const std::string srv_implementation1("sight::service::ut::test_no_data");
        const std::string srv_implementation2("sight::service::ut::test_no_data2");

        sight::data::integer::sptr obj1 = std::make_shared<sight::data::integer>();
        sight::data::integer::sptr obj2 = std::make_shared<sight::data::integer>();
        sight::data::integer::sptr obj3 = std::make_shared<sight::data::integer>();

        auto service1 = sight::service::extension::factory::get()->create(srv_implementation1);
        auto service2 = sight::service::extension::factory::get()->create(srv_implementation2);
        auto service3 = sight::service::extension::factory::get()->create(srv_implementation1);

        sight::service::register_service(service1);
        sight::service::register_service(service2);
        sight::service::register_service(service3);

        // 3 services in total
        {
            auto services_by_type          = sight::service::get_services(srv_type);
            auto services_by_template_type = sight::service::get_services<sight::service::ut::test_srv>();

            CHECK_EQ(std::size_t(3), services_by_type.size());
            CHECK(std::equal(services_by_type.begin(), services_by_type.end(), services_by_template_type.begin()));
        }

        // 2 services of type "sight::service::ut::test_no_data"
        {
            auto services_by_type          = sight::service::get_services(srv_implementation1);
            auto services_by_template_type = sight::service::get_services<sight::service::ut::test_no_data>();

            CHECK_EQ(std::size_t(2), services_by_type.size());
            CHECK(std::equal(services_by_type.begin(), services_by_type.end(), services_by_template_type.begin()));
        }

        // 1 service of type "sight::service::ut::test_no_data2"
        {
            auto services_by_type          = sight::service::get_services(srv_implementation2);
            auto services_by_template_type = sight::service::get_services<sight::service::ut::test_no_data2>();

            CHECK_EQ(std::size_t(1), services_by_type.size());
            CHECK(std::equal(services_by_type.begin(), services_by_type.end(), services_by_template_type.begin()));
        }

        auto services_by_type = sight::service::get_services(srv_type);
        CHECK_EQ(std::size_t(3), services_by_type.size());

        CHECK(services_by_type.find(service1) != services_by_type.end());
        CHECK(services_by_type.find(service2) != services_by_type.end());
        CHECK(services_by_type.find(service3) != services_by_type.end());

        // Remove key 1 from service 1 and check consistency
        services_by_type = sight::service::get_services(srv_type);
        CHECK_EQ(std::size_t(3), services_by_type.size());

        sight::service::unregister_service(service1);
        services_by_type = sight::service::get_services(srv_type);
        CHECK_EQ(std::size_t(2), services_by_type.size());

        services_by_type = sight::service::get_services(srv_type);
        CHECK_EQ(std::size_t(2), services_by_type.size());

        sight::service::register_service(service1);
        services_by_type = sight::service::get_services(srv_type);
        CHECK_EQ(std::size_t(3), services_by_type.size());

        // Remove service 3 and check consistency
        sight::service::unregister_service(service3);
        services_by_type = sight::service::get_services(srv_type);
        CHECK_EQ(std::size_t(2), services_by_type.size());

        sight::service::unregister_service(service2);

        services_by_type = sight::service::get_services(srv_type);
        CHECK_EQ(std::size_t(1), services_by_type.size());

        sight::service::unregister_service(service1);
        services_by_type = sight::service::get_services(srv_type);
        CHECK(services_by_type.empty());
    }
}
