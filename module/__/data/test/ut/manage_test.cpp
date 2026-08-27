/************************************************************************
 *
 * Copyright (C) 2022-2026 IRCAD France
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

#include <data/map.hpp>
#include <data/series.hpp>
#include <data/series_set.hpp>
#include <data/string.hpp>
#include <data/vector.hpp>

#include <utest/service_fixture.hpp>

namespace
{

class service_fixture : public sight::utest::service_fixture
{
public:

    service_fixture() :
        sight::utest::service_fixture("sight::module::data::manage")
    {
    }

    //------------------------------------------------------------------------------

    void generic_add_in_map_test(const std::string& _slot_name, bool _already_present = false)
    {
        auto object    = std::make_shared<sight::data::string>("Hello world");
        auto container = std::make_shared<sight::data::map>();
        if(_already_present)
        {
            (*container)["myKey"] = object;
        }

        m_service->set_inout(object, "data.element");
        m_service->set_inout(container, "data.container");
        boost::property_tree::ptree ptree;
        ptree.put("mapKey", "myKey");
        m_service->set_config(ptree);
        CHECK_NOTHROW(m_service->configure());
        CHECK_NOTHROW(m_service->start().get());

        m_service->slot(_slot_name)->run();

        if(_slot_name == "add_copy")
        {
            CHECK(object != container->get<sight::data::string>("myKey"));
            CHECK_EQ(*object, *container->get<sight::data::string>("myKey"));
        }
        else
        {
            CHECK_EQ(object, container->get<sight::data::string>("myKey"));
        }
    }

//------------------------------------------------------------------------------

    void generic_add_in_vector_test(const std::string& _slot_name, bool _already_present = false)
    {
        auto object    = std::make_shared<sight::data::string>("Hello world");
        auto container = std::make_shared<sight::data::vector>();
        if(_already_present)
        {
            container->push_back(object);
        }

        m_service->set_inout(object, "data.element");
        m_service->set_inout(container, "data.container");
        CHECK_NOTHROW(m_service->configure());
        CHECK_NOTHROW(m_service->start().get());

        m_service->slot(_slot_name)->run();

        CHECK_EQ(std::size_t(1), container->size());
        if(_slot_name == "add_copy")
        {
            CHECK(object != std::dynamic_pointer_cast<sight::data::string>((*container)[0]));
            CHECK_EQ(*object, *std::dynamic_pointer_cast<sight::data::string>((*container)[0]));
        }
        else
        {
            CHECK_EQ(object, std::dynamic_pointer_cast<sight::data::string>((*container)[0]));
        }

        m_service->slot(_slot_name)->run();

        std::size_t expected_size = 2 - static_cast<std::size_t>(_slot_name == "add_or_swap");
        CHECK_EQ(expected_size, container->size());
        if(_slot_name == "add_copy")
        {
            CHECK(object != std::dynamic_pointer_cast<sight::data::string>((*container)[1]));
            CHECK_EQ(*object, *std::dynamic_pointer_cast<sight::data::string>((*container)[1]));
        }
        else
        {
            CHECK_EQ(object, std::dynamic_pointer_cast<sight::data::string>((*container)[expected_size - 1]));
        }
    }

//------------------------------------------------------------------------------

    void generic_add_in_series_set_test(const std::string& _slot_name, bool _already_present = false)
    {
        auto object = std::make_shared<sight::data::series>();
        object->set_patient_name("You");
        auto container = std::make_shared<sight::data::series_set>();
        if(_already_present)
        {
            container->push_back(object);
        }

        m_service->set_inout(object, "data.element");
        m_service->set_inout(container, "data.container");
        CHECK_NOTHROW(m_service->configure());
        CHECK_NOTHROW(m_service->start().get());

        m_service->slot(_slot_name)->run();

        CHECK_EQ(std::size_t(1), container->size());
        if(_slot_name == "add_copy")
        {
            CHECK(object != (*container)[0]);
            CHECK(*object == *(*container)[0]);
        }
        else
        {
            CHECK_EQ(object, (*container)[0]);
        }

        m_service->slot(_slot_name)->run();

        if(_slot_name == "add_copy")
        {
            CHECK_EQ(std::size_t(2), container->size());
            CHECK(object != (*container)[0]);
            CHECK(*object == *(*container)[0]);
        }
        else
        {
            CHECK_EQ(std::size_t(1), container->size());
            CHECK_EQ(object, (*container)[0]);
        }
    }

//------------------------------------------------------------------------------

    void generic_add_in_field_test(const std::string& _slot_name, bool _already_present = false)
    {
        auto object    = std::make_shared<sight::data::string>("Hello world");
        auto container = std::make_shared<sight::data::string>();
        if(_already_present)
        {
            container->set_field("myField", object);
        }

        m_service->set_inout(object, "data.element");
        m_service->set_inout(container, "data.container");
        boost::property_tree::ptree ptree;
        ptree.put("field", "myField");
        m_service->set_config(ptree);
        CHECK_NOTHROW(m_service->configure());
        CHECK_NOTHROW(m_service->start().get());

        m_service->slot(_slot_name)->run();

        if(_slot_name == "add_copy")
        {
            CHECK(object != container->get_field<sight::data::string>("myField"));
            CHECK_EQ(*object, *container->get_field<sight::data::string>("myField"));
        }
        else
        {
            CHECK_EQ(object, container->get_field<sight::data::string>("myField"));
        }
    }

//------------------------------------------------------------------------------

    void generic_remove_in_map_test(const std::string& _slot_name)
    {
        auto object    = std::make_shared<sight::data::string>("Hello world");
        auto container = std::make_shared<sight::data::map>();
        (*container)["myKey"] = object;

        m_service->set_inout(object, "data.element");
        m_service->set_inout(container, "data.container");
        boost::property_tree::ptree ptree;
        ptree.put("mapKey", "myKey");
        m_service->set_config(ptree);
        CHECK_NOTHROW(m_service->configure());
        CHECK_NOTHROW(m_service->start().get());

        CHECK(container->get<sight::data::string>("myKey") != nullptr);
        m_service->slot(_slot_name)->run();
        CHECK(container->get<sight::data::string>("myKey") == nullptr);
    }

//------------------------------------------------------------------------------

    void generic_remove_in_vector_test(const std::string& _slot_name)
    {
        auto object    = std::make_shared<sight::data::string>("Hello world");
        auto container = std::make_shared<sight::data::vector>();
        container->push_back(object);

        m_service->set_inout(object, "data.element");
        m_service->set_inout(container, "data.container");
        CHECK_NOTHROW(m_service->configure());
        CHECK_NOTHROW(m_service->start().get());

        CHECK(!container->empty());
        m_service->slot(_slot_name)->run();
        CHECK(container->empty());
    }

//------------------------------------------------------------------------------

    void generic_remove_in_series_set_test(const std::string& _slot_name)
    {
        auto object = std::make_shared<sight::data::series>();
        object->set_patient_name("You");
        auto container = std::make_shared<sight::data::series_set>();
        container->push_back(object);

        m_service->set_inout(object, "data.element");
        m_service->set_inout(container, "data.container");
        CHECK_NOTHROW(m_service->configure());
        CHECK_NOTHROW(m_service->start().get());

        CHECK(!container->empty());
        m_service->slot(_slot_name)->run();
        CHECK(container->empty());
    }

//------------------------------------------------------------------------------

    void generic_remove_in_field_test(const std::string& _slot_name)
    {
        auto object    = std::make_shared<sight::data::string>("Hello world");
        auto container = std::make_shared<sight::data::string>();
        container->set_field("myField", object);

        m_service->set_inout(object, "data.element");
        m_service->set_inout(container, "data.container");
        boost::property_tree::ptree ptree;
        ptree.put("field", "myField");
        m_service->set_config(ptree);
        CHECK_NOTHROW(m_service->configure());
        CHECK_NOTHROW(m_service->start().get());

        CHECK_EQ(object, container->get_field<sight::data::string>("myField"));
        m_service->slot(_slot_name)->run();
        CHECK(container->get_field<sight::data::string>("myField") == nullptr);
    }
};

} // namespace
TEST_SUITE("sight::module::data::manage")
{
    TEST_CASE_FIXTURE(service_fixture, "add_in_map")
    {
        generic_add_in_map_test("add");
    }

    TEST_CASE_FIXTURE(service_fixture, "add_in_vector")
    {
        generic_add_in_vector_test("add");
    }

    TEST_CASE_FIXTURE(service_fixture, "add_in_series_set")
    {
        generic_add_in_series_set_test("add");
    }

    TEST_CASE_FIXTURE(service_fixture, "add_in_field")
    {
        generic_add_in_field_test("add");
    }

    TEST_CASE_FIXTURE(service_fixture, "add_copy_in_map")
    {
        generic_add_in_map_test("add_copy");
    }

    TEST_CASE_FIXTURE(service_fixture, "add_copy_in_vector")
    {
        generic_add_in_vector_test("add_copy");
    }

    TEST_CASE_FIXTURE(service_fixture, "add_copy_in_series_set")
    {
        generic_add_in_series_set_test("add_copy");
    }

    TEST_CASE_FIXTURE(service_fixture, "add_copy_in_field")
    {
        generic_add_in_field_test("add_copy");
    }

    TEST_CASE_FIXTURE(service_fixture, "add_or_swap_and_not_present_in_map")
    {
        generic_add_in_map_test("add_or_swap");
    }

    TEST_CASE_FIXTURE(service_fixture, "add_or_swap_and_not_present_in_vector")
    {
        generic_add_in_vector_test("add_or_swap");
    }

    TEST_CASE_FIXTURE(service_fixture, "add_or_swap_and_not_present_in_series_set")
    {
        generic_add_in_series_set_test("add_or_swap");
    }

    TEST_CASE_FIXTURE(service_fixture, "add_or_swap_and_not_present_in_field")
    {
        generic_add_in_field_test("add_or_swap");
    }

    TEST_CASE_FIXTURE(service_fixture, "add_or_swap_and_present_in_map")
    {
        generic_add_in_map_test("add_or_swap", true);
    }

    TEST_CASE_FIXTURE(service_fixture, "add_or_swap_and_present_in_vector")
    {
        generic_add_in_vector_test("add_or_swap", true);
    }

    TEST_CASE_FIXTURE(service_fixture, "add_or_swap_and_present_in_series_set")
    {
        generic_add_in_series_set_test("add_or_swap", true);
    }

    TEST_CASE_FIXTURE(service_fixture, "add_or_swap_and_present_in_field")
    {
        generic_add_in_field_test("add_or_swap", true);
    }

    TEST_CASE_FIXTURE(service_fixture, "swap_obj_in_map")
    {
        generic_add_in_map_test("swap_obj");
    }

    TEST_CASE_FIXTURE(service_fixture, "swap_obj_and_not_present_in_field")
    {
        CHECK_THROWS_AS(generic_add_in_field_test("swap_obj"), sight::data::exception);
    }

    TEST_CASE_FIXTURE(service_fixture, "swap_obj_and_present_in_field")
    {
        generic_add_in_field_test("swap_obj", true);
    }

    TEST_CASE_FIXTURE(service_fixture, "pop_front_in_map")
    {
        generic_remove_in_map_test("pop_front");
    }

    TEST_CASE_FIXTURE(service_fixture, "pop_front_in_vector")
    {
        generic_remove_in_vector_test("pop_front");
    }

    TEST_CASE_FIXTURE(service_fixture, "pop_front_in_series_set")
    {
        generic_remove_in_series_set_test("pop_front");
    }

    TEST_CASE_FIXTURE(service_fixture, "pop_front_in_field")
    {
        generic_remove_in_field_test("pop_front");
    }

    TEST_CASE_FIXTURE(service_fixture, "remove_in_map")
    {
        generic_remove_in_map_test("remove");
    }

    TEST_CASE_FIXTURE(service_fixture, "remove_in_vector")
    {
        generic_remove_in_vector_test("remove");
    }

    TEST_CASE_FIXTURE(service_fixture, "remove_in_series_set")
    {
        generic_remove_in_series_set_test("remove");
    }

    TEST_CASE_FIXTURE(service_fixture, "remove_in_field")
    {
        generic_remove_in_field_test("remove");
    }

    TEST_CASE_FIXTURE(service_fixture, "remove_if_present_in_map")
    {
        generic_remove_in_map_test("remove_if_present");
    }

    TEST_CASE_FIXTURE(service_fixture, "remove_if_present_in_vector")
    {
        m_service->start().get();
        //genericRemoveInVectorTest("remove_if_present"); // TODO: fix crash
    }

    TEST_CASE_FIXTURE(service_fixture, "remove_if_present_in_series_set")
    {
        generic_remove_in_series_set_test("remove_if_present");
    }

    TEST_CASE_FIXTURE(service_fixture, "remove_if_present_in_field")
    {
        generic_remove_in_field_test("remove_if_present");
    }

    TEST_CASE_FIXTURE(service_fixture, "clear_map")
    {
        generic_remove_in_map_test("clear");
    }

    TEST_CASE_FIXTURE(service_fixture, "clear_vector")
    {
        generic_remove_in_vector_test("clear");
    }

    TEST_CASE_FIXTURE(service_fixture, "clear_series_set")
    {
        generic_remove_in_series_set_test("clear");
    }

    TEST_CASE_FIXTURE(service_fixture, "clear_field")
    {
        generic_remove_in_field_test("clear");
    }
} // TEST_SUITE("sight::module::data::manage")
