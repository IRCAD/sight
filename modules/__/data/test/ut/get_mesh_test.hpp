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

#pragma once

#include <cppunit/extensions/HelperMacros.h>

namespace sight::module::data::ut
{

/**
 * @brief Test Extract Mesh from Model series
 */
class get_mesh_test : public CPPUNIT_NS::TestFixture
{
CPPUNIT_TEST_SUITE(get_mesh_test);
CPPUNIT_TEST(extracts_mesh_by_index);
CPPUNIT_TEST(extracts_mesh_with_index_out_of_bound);
CPPUNIT_TEST(extracts_mesh_by_type);
CPPUNIT_TEST(extracts_mesh_by_type_not_found);
CPPUNIT_TEST(extracts_mesh_by_name_not_found);
CPPUNIT_TEST(extracts_with_invalid_models_series);
CPPUNIT_TEST(extracts_with_mesh_type_and_index);
CPPUNIT_TEST_SUITE_END();

public:

    // interface
    void setUp() override;
    void tearDown() override;

    static void extracts_mesh_by_index();
    static void extracts_mesh_by_type();
    static void extracts_mesh_by_type_not_found();
    static void extracts_mesh_by_name_not_found();
    static void extracts_with_invalid_models_series();
    static void extracts_mesh_with_index_out_of_bound();
    static void extracts_with_mesh_type_and_index();
};

} // namespace sight::module::data::ut
