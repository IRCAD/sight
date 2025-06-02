/************************************************************************
 *
 * Copyright (C) 2022-2025 IRCAD France
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

#include <service/base.hpp>

#include <cppunit/extensions/HelperMacros.h>

namespace sight::module::data::ut
{

class manage_test : public CPPUNIT_NS::TestFixture
{
CPPUNIT_TEST_SUITE(manage_test);
CPPUNIT_TEST(add_in_map_test);
CPPUNIT_TEST(add_in_vector_test);
CPPUNIT_TEST(add_in_series_set_test);
CPPUNIT_TEST(add_in_field_test);

CPPUNIT_TEST(add_copy_in_map_test);
CPPUNIT_TEST(add_copy_in_vector_test);
CPPUNIT_TEST(add_copy_in_series_set_test);
CPPUNIT_TEST(add_copy_in_field_test);

CPPUNIT_TEST(add_or_swap_and_not_present_in_map_test);
CPPUNIT_TEST(add_or_swap_and_not_present_in_vector_test);
CPPUNIT_TEST(add_or_swap_and_not_present_in_series_set_test);
CPPUNIT_TEST(add_or_swap_and_not_present_in_field_test);

CPPUNIT_TEST(add_or_swap_and_present_in_map_test);
CPPUNIT_TEST(add_or_swap_and_present_in_vector_test);
CPPUNIT_TEST(add_or_swap_and_present_in_series_set_test);
CPPUNIT_TEST(add_or_swap_and_present_in_field_test);

CPPUNIT_TEST(pop_front_in_map_test);
CPPUNIT_TEST(pop_front_in_vector_test);
CPPUNIT_TEST(pop_front_in_series_set_test);
CPPUNIT_TEST(pop_front_in_field_test);

CPPUNIT_TEST(swap_obj_in_map_test);
CPPUNIT_TEST(swap_obj_and_not_present_in_field_test);
CPPUNIT_TEST(swap_obj_and_present_in_field_test);

CPPUNIT_TEST(remove_in_map_test);
CPPUNIT_TEST(remove_in_vector_test);
CPPUNIT_TEST(remove_in_series_set_test);
CPPUNIT_TEST(remove_in_field_test);

CPPUNIT_TEST(remove_if_present_in_map_test);
CPPUNIT_TEST(remove_if_present_in_vector_test);
CPPUNIT_TEST(remove_if_present_in_series_set_test);
CPPUNIT_TEST(remove_if_present_in_field_test);

CPPUNIT_TEST(clear_map_test);
CPPUNIT_TEST(clear_vector_test);
CPPUNIT_TEST(clear_series_set_test);
CPPUNIT_TEST(clear_field_test);
CPPUNIT_TEST_SUITE_END();

public:

    void setUp() override;
    void tearDown() override;

    void add_in_map_test();
    void add_in_vector_test();
    void add_in_series_set_test();
    void add_in_field_test();

    void add_copy_in_map_test();
    void add_copy_in_vector_test();
    void add_copy_in_series_set_test();
    void add_copy_in_field_test();

    void add_or_swap_and_not_present_in_map_test();
    void add_or_swap_and_not_present_in_vector_test();
    void add_or_swap_and_not_present_in_series_set_test();
    void add_or_swap_and_not_present_in_field_test();

    void add_or_swap_and_present_in_map_test();
    void add_or_swap_and_present_in_vector_test();
    void add_or_swap_and_present_in_series_set_test();
    void add_or_swap_and_present_in_field_test();

    void pop_front_in_map_test();
    void pop_front_in_vector_test();
    void pop_front_in_series_set_test();
    void pop_front_in_field_test();

    void swap_obj_in_map_test();
    void swap_obj_and_not_present_in_field_test();
    void swap_obj_and_present_in_field_test();

    void remove_in_map_test();
    void remove_in_vector_test();
    void remove_in_series_set_test();
    void remove_in_field_test();

    void remove_if_present_in_map_test();
    void remove_if_present_in_vector_test();
    void remove_if_present_in_series_set_test();
    void remove_if_present_in_field_test();

    void clear_map_test();
    void clear_vector_test();
    void clear_series_set_test();
    void clear_field_test();

private:

    void generic_add_in_map_test(const std::string& _slot_name, bool _already_present        = false);
    void generic_add_in_vector_test(const std::string& _slot_name, bool _already_present     = false);
    void generic_add_in_series_set_test(const std::string& _slot_name, bool _already_present = false);
    void generic_add_in_field_test(const std::string& _slot_name, bool _already_present      = false);

    void generic_remove_in_map_test(const std::string& _slot_name);
    void generic_remove_in_vector_test(const std::string& _slot_name);
    void generic_remove_in_series_set_test(const std::string& _slot_name);
    void generic_remove_in_field_test(const std::string& _slot_name);

    service::base::sptr m_manage;
};

} // namespace sight::module::data::ut
