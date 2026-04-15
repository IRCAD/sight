/************************************************************************
 *
 * Copyright (C) 2018-2026 IRCAD France
 * Copyright (C) 2018-2019 IHU Strasbourg
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
#include <service/macros.hpp>
#include <service/op.hpp>

#include <doctest/doctest.h>

#include <algorithm>

namespace sight::service::ut
{

class test_empty_service : public sight::service::base
{
public:

    //------------------------------------------------------------------------------

    void configuring() override
    {
    }

    //------------------------------------------------------------------------------

    void starting() override
    {
    }

    //------------------------------------------------------------------------------

    void stopping() override
    {
    }

    //------------------------------------------------------------------------------

    void updating() override
    {
    }
};

class test_srv_one_data : public test_empty_service
{
public:

    SIGHT_DECLARE_SERVICE(test_srv_one_data, sight::service::base);
};
class test_srv_three_data : public test_empty_service
{
public:

    SIGHT_DECLARE_SERVICE(test_srv_three_data, sight::service::base);
};

SIGHT_REGISTER_SERVICE(
    sight::service::ut::test_service,
    sight::service::ut::test_srv_one_data,
    sight::data::real
)
SIGHT_REGISTER_SERVICE(sight::service::ut::test_service, sight::service::ut::test_srv_three_data);
SIGHT_REGISTER_SERVICE_OBJECT(sight::service::ut::test_srv_three_data, sight::data::real);
SIGHT_REGISTER_SERVICE_OBJECT(sight::service::ut::test_srv_three_data, sight::data::integer);
SIGHT_REGISTER_SERVICE_OBJECT(sight::service::ut::test_srv_three_data, sight::data::real);

} // namespace sight::service::ut

TEST_SUITE("sight::service::service_factory")
{
    TEST_CASE("factory")
    {
        const auto service_factory = sight::service::extension::factory::get();
        {
            const auto services_impl = service_factory->get_implementation_id_from_object_and_type(
                "sight::data::real",
                "sight::service::ut::test_service"
            );

            auto it = std::ranges::find(services_impl, "sight::service::ut::test_srv_one_data");
            CHECK_MESSAGE(it != services_impl.end(), "sight::service::ut::test_srv_one_data not found");

            const auto objects = service_factory->get_service_objects("sight::service::ut::test_srv_one_data");
            CHECK_EQ(std::string("sight::data::real"), objects[0]);
        }
        {
            auto services_impl = service_factory->get_implementation_id_from_object_and_type(
                "sight::data::real",
                "sight::service::ut::test_service"
            );

            auto it = std::ranges::find(services_impl, "sight::service::ut::test_srv_three_data");
            CHECK_MESSAGE(it != services_impl.end(), "sight::service::ut::test_srv_three_data not found");

            services_impl = service_factory->get_implementation_id_from_object_and_type(
                "sight::data::integer",
                "sight::service::ut::test_service"
            );

            it = std::ranges::find(services_impl, "sight::service::ut::test_srv_three_data");
            CHECK_MESSAGE(it != services_impl.end(), "sight::service::ut::test_srv_three_data not found");

            const auto objects = service_factory->get_service_objects("sight::service::ut::test_srv_three_data");
            CHECK_EQ(static_cast<std::size_t>(3), objects.size());
            CHECK_EQ(std::string("sight::data::real"), objects[0]);
            CHECK_EQ(std::string("sight::data::integer"), objects[1]);
            CHECK_EQ(std::string("sight::data::real"), objects[2]);
        }
    }
} // end TEST_SUITE
