/************************************************************************
 *
 * Copyright (C) 2022-2023 IRCAD France
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

#include "manage_test.hpp"

#include <data/composite.hpp>
#include <data/series.hpp>
#include <data/series_set.hpp>
#include <data/string.hpp>
#include <data/vector.hpp>

#include <service/op.hpp>

CPPUNIT_TEST_SUITE_REGISTRATION(sight::module::data::ut::manage_test);

namespace sight::module::data::ut
{

//------------------------------------------------------------------------------

void manage_test::setUp()
{
    m_manage = service::add("sight::module::data::manage");
}

//------------------------------------------------------------------------------

void manage_test::tearDown()
{
    CPPUNIT_ASSERT_NO_THROW(m_manage->stop().get());
    service::remove(m_manage);
}

//------------------------------------------------------------------------------

void manage_test::generic_add_in_composite_test(const std::string& _slot_name, bool _already_present)
{
    auto object    = std::make_shared<sight::data::string>("Hello world");
    auto container = std::make_shared<sight::data::composite>();
    if(_already_present)
    {
        (*container)["myKey"] = object;
    }

    m_manage->set_inout(object, "object");
    m_manage->set_inout(container, "container");
    boost::property_tree::ptree ptree;
    ptree.put("compositeKey", "myKey");
    m_manage->set_config(ptree);
    CPPUNIT_ASSERT_NO_THROW(m_manage->configure());
    CPPUNIT_ASSERT_NO_THROW(m_manage->start().get());

    m_manage->slot(_slot_name)->run();

    if(_slot_name == "addCopy")
    {
        CPPUNIT_ASSERT(object != container->get<sight::data::string>("myKey"));
        CPPUNIT_ASSERT_EQUAL(*object, *container->get<sight::data::string>("myKey"));
    }
    else
    {
        CPPUNIT_ASSERT_EQUAL(object, container->get<sight::data::string>("myKey"));
    }
}

//------------------------------------------------------------------------------

void manage_test::generic_add_in_vector_test(const std::string& _slot_name, bool _already_present)
{
    auto object    = std::make_shared<sight::data::string>("Hello world");
    auto container = std::make_shared<sight::data::vector>();
    if(_already_present)
    {
        container->push_back(object);
    }

    m_manage->set_inout(object, "object");
    m_manage->set_inout(container, "container");
    CPPUNIT_ASSERT_NO_THROW(m_manage->configure());
    CPPUNIT_ASSERT_NO_THROW(m_manage->start().get());

    m_manage->slot(_slot_name)->run();

    CPPUNIT_ASSERT_EQUAL(std::size_t(1), container->size());
    if(_slot_name == "addCopy")
    {
        CPPUNIT_ASSERT(object != std::dynamic_pointer_cast<sight::data::string>((*container)[0]));
        CPPUNIT_ASSERT_EQUAL(*object, *std::dynamic_pointer_cast<sight::data::string>((*container)[0]));
    }
    else
    {
        CPPUNIT_ASSERT_EQUAL(object, std::dynamic_pointer_cast<sight::data::string>((*container)[0]));
    }

    m_manage->slot(_slot_name)->run();

    std::size_t expected_size = 2 - static_cast<std::size_t>(_slot_name == "addOrSwap");
    CPPUNIT_ASSERT_EQUAL(expected_size, container->size());
    if(_slot_name == "addCopy")
    {
        CPPUNIT_ASSERT(object != std::dynamic_pointer_cast<sight::data::string>((*container)[1]));
        CPPUNIT_ASSERT_EQUAL(*object, *std::dynamic_pointer_cast<sight::data::string>((*container)[1]));
    }
    else
    {
        CPPUNIT_ASSERT_EQUAL(object, std::dynamic_pointer_cast<sight::data::string>((*container)[expected_size - 1]));
    }
}

//------------------------------------------------------------------------------

void manage_test::generic_add_in_series_set_test(const std::string& _slot_name, bool _already_present)
{
    auto object = std::make_shared<sight::data::series>();
    object->set_patient_name("You");
    auto container = std::make_shared<sight::data::series_set>();
    if(_already_present)
    {
        container->push_back(object);
    }

    m_manage->set_inout(object, "object");
    m_manage->set_inout(container, "container");
    CPPUNIT_ASSERT_NO_THROW(m_manage->configure());
    CPPUNIT_ASSERT_NO_THROW(m_manage->start().get());

    m_manage->slot(_slot_name)->run();

    CPPUNIT_ASSERT_EQUAL(std::size_t(1), container->size());
    if(_slot_name == "addCopy")
    {
        CPPUNIT_ASSERT(object != (*container)[0]);
        CPPUNIT_ASSERT(*object == *(*container)[0]);
    }
    else
    {
        CPPUNIT_ASSERT_EQUAL(object, (*container)[0]);
    }

    m_manage->slot(_slot_name)->run();

    if(_slot_name == "addCopy")
    {
        CPPUNIT_ASSERT_EQUAL(std::size_t(2), container->size());
        CPPUNIT_ASSERT(object != (*container)[0]);
        CPPUNIT_ASSERT(*object == *(*container)[0]);
    }
    else
    {
        CPPUNIT_ASSERT_EQUAL(std::size_t(1), container->size());
        CPPUNIT_ASSERT_EQUAL(object, (*container)[0]);
    }
}

//------------------------------------------------------------------------------

void manage_test::generic_add_in_field_test(const std::string& _slot_name, bool _already_present)
{
    auto object    = std::make_shared<sight::data::string>("Hello world");
    auto container = std::make_shared<sight::data::string>();
    if(_already_present)
    {
        container->set_field("myField", object);
    }

    m_manage->set_inout(object, "object");
    m_manage->set_inout(container, "container");
    boost::property_tree::ptree ptree;
    ptree.put("field", "myField");
    m_manage->set_config(ptree);
    CPPUNIT_ASSERT_NO_THROW(m_manage->configure());
    CPPUNIT_ASSERT_NO_THROW(m_manage->start().get());

    m_manage->slot(_slot_name)->run();

    if(_slot_name == "addCopy")
    {
        CPPUNIT_ASSERT(object != container->get_field<sight::data::string>("myField"));
        CPPUNIT_ASSERT_EQUAL(*object, *container->get_field<sight::data::string>("myField"));
    }
    else
    {
        CPPUNIT_ASSERT_EQUAL(object, container->get_field<sight::data::string>("myField"));
    }
}

//------------------------------------------------------------------------------

void manage_test::add_in_composite_test()
{
    generic_add_in_composite_test("add");
}

//------------------------------------------------------------------------------

void manage_test::add_in_vector_test()
{
    generic_add_in_vector_test("add");
}

//------------------------------------------------------------------------------

void manage_test::add_in_series_set_test()
{
    generic_add_in_series_set_test("add");
}

//------------------------------------------------------------------------------

void manage_test::add_in_field_test()
{
    generic_add_in_field_test("add");
}

//------------------------------------------------------------------------------

void manage_test::add_copy_in_composite_test()
{
    generic_add_in_composite_test("addCopy");
}

//------------------------------------------------------------------------------

void manage_test::add_copy_in_vector_test()
{
    generic_add_in_vector_test("addCopy");
}

//------------------------------------------------------------------------------

void manage_test::add_copy_in_series_set_test()
{
    generic_add_in_series_set_test("addCopy");
}

//------------------------------------------------------------------------------

void manage_test::add_copy_in_field_test()
{
    generic_add_in_field_test("addCopy");
}

//------------------------------------------------------------------------------

void manage_test::add_or_swap_and_not_present_in_composite_test()
{
    generic_add_in_composite_test("addOrSwap");
}

//------------------------------------------------------------------------------

void manage_test::add_or_swap_and_not_present_in_vector_test()
{
    generic_add_in_vector_test("addOrSwap");
}

//------------------------------------------------------------------------------

void manage_test::add_or_swap_and_not_present_in_series_set_test()
{
    generic_add_in_series_set_test("addOrSwap");
}

//------------------------------------------------------------------------------

void manage_test::add_or_swap_and_not_present_in_field_test()
{
    generic_add_in_field_test("addOrSwap");
}

//------------------------------------------------------------------------------

void manage_test::add_or_swap_and_present_in_composite_test()
{
    generic_add_in_composite_test("addOrSwap", true);
}

//------------------------------------------------------------------------------

void manage_test::add_or_swap_and_present_in_vector_test()
{
    generic_add_in_vector_test("addOrSwap", true);
}

//------------------------------------------------------------------------------

void manage_test::add_or_swap_and_present_in_series_set_test()
{
    generic_add_in_series_set_test("addOrSwap", true);
}

//------------------------------------------------------------------------------

void manage_test::add_or_swap_and_present_in_field_test()
{
    generic_add_in_field_test("addOrSwap", true);
}

//------------------------------------------------------------------------------

void manage_test::swap_obj_in_composite_test()
{
    generic_add_in_composite_test("swapObj");
}

//------------------------------------------------------------------------------

void manage_test::swap_obj_and_not_present_in_field_test()
{
    CPPUNIT_ASSERT_THROW(generic_add_in_field_test("swapObj"), sight::data::exception);
}

//------------------------------------------------------------------------------

void manage_test::swap_obj_and_present_in_field_test()
{
    generic_add_in_field_test("swapObj", true);
}

//------------------------------------------------------------------------------

void manage_test::generic_remove_in_composite_test(const std::string& _slot_name)
{
    auto object    = std::make_shared<sight::data::string>("Hello world");
    auto container = std::make_shared<sight::data::composite>();
    (*container)["myKey"] = object;

    m_manage->set_inout(object, "object");
    m_manage->set_inout(container, "container");
    boost::property_tree::ptree ptree;
    ptree.put("compositeKey", "myKey");
    m_manage->set_config(ptree);
    CPPUNIT_ASSERT_NO_THROW(m_manage->configure());
    CPPUNIT_ASSERT_NO_THROW(m_manage->start().get());

    CPPUNIT_ASSERT(container->get<sight::data::string>("myKey") != nullptr);
    m_manage->slot(_slot_name)->run();
    CPPUNIT_ASSERT(container->get<sight::data::string>("myKey") == nullptr);
}

//------------------------------------------------------------------------------

void manage_test::generic_remove_in_vector_test(const std::string& _slot_name)
{
    auto object    = std::make_shared<sight::data::string>("Hello world");
    auto container = std::make_shared<sight::data::vector>();
    container->push_back(object);

    m_manage->set_inout(object, "object");
    m_manage->set_inout(container, "container");
    CPPUNIT_ASSERT_NO_THROW(m_manage->configure());
    CPPUNIT_ASSERT_NO_THROW(m_manage->start().get());

    CPPUNIT_ASSERT(!container->empty());
    m_manage->slot(_slot_name)->run();
    CPPUNIT_ASSERT(container->empty());
}

//------------------------------------------------------------------------------

void manage_test::generic_remove_in_series_set_test(const std::string& _slot_name)
{
    auto object = std::make_shared<sight::data::series>();
    object->set_patient_name("You");
    auto container = std::make_shared<sight::data::series_set>();
    container->push_back(object);

    m_manage->set_inout(object, "object");
    m_manage->set_inout(container, "container");
    CPPUNIT_ASSERT_NO_THROW(m_manage->configure());
    CPPUNIT_ASSERT_NO_THROW(m_manage->start().get());

    CPPUNIT_ASSERT(!container->empty());
    m_manage->slot(_slot_name)->run();
    CPPUNIT_ASSERT(container->empty());
}

//------------------------------------------------------------------------------

void manage_test::generic_remove_in_field_test(const std::string& _slot_name)
{
    auto object    = std::make_shared<sight::data::string>("Hello world");
    auto container = std::make_shared<sight::data::string>();
    container->set_field("myField", object);

    m_manage->set_inout(object, "object");
    m_manage->set_inout(container, "container");
    boost::property_tree::ptree ptree;
    ptree.put("field", "myField");
    m_manage->set_config(ptree);
    CPPUNIT_ASSERT_NO_THROW(m_manage->configure());
    CPPUNIT_ASSERT_NO_THROW(m_manage->start().get());

    CPPUNIT_ASSERT_EQUAL(object, container->get_field<sight::data::string>("myField"));
    m_manage->slot(_slot_name)->run();
    CPPUNIT_ASSERT(container->get_field<sight::data::string>("myField") == nullptr);
}

//------------------------------------------------------------------------------

void manage_test::remove_in_composite_test()
{
    generic_remove_in_composite_test("remove");
}

//------------------------------------------------------------------------------

void manage_test::remove_in_vector_test()
{
    generic_remove_in_vector_test("remove");
}

//------------------------------------------------------------------------------

void manage_test::remove_in_series_set_test()
{
    generic_remove_in_series_set_test("remove");
}

//------------------------------------------------------------------------------

void manage_test::remove_in_field_test()
{
    generic_remove_in_field_test("remove");
}

//------------------------------------------------------------------------------

void manage_test::remove_if_present_in_composite_test()
{
    generic_remove_in_composite_test("removeIfPresent");
}

//------------------------------------------------------------------------------

void manage_test::remove_if_present_in_vector_test()
{
    m_manage->start().get();
    //genericRemoveInVectorTest("removeIfPresent"); // TODO: fix crash
}

//------------------------------------------------------------------------------

void manage_test::remove_if_present_in_series_set_test()
{
    generic_remove_in_series_set_test("removeIfPresent");
}

//------------------------------------------------------------------------------

void manage_test::remove_if_present_in_field_test()
{
    generic_remove_in_field_test("removeIfPresent");
}

//------------------------------------------------------------------------------

void manage_test::clear_composite_test()
{
    generic_remove_in_composite_test("clear");
}

//------------------------------------------------------------------------------

void manage_test::clear_vector_test()
{
    generic_remove_in_vector_test("clear");
}

//------------------------------------------------------------------------------

void manage_test::clear_series_set_test()
{
    generic_remove_in_series_set_test("clear");
}

//------------------------------------------------------------------------------

void manage_test::clear_field_test()
{
    generic_remove_in_field_test("clear");
}

} // namespace sight::module::data::ut
