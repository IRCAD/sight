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
 * @brief Test Extract image series from Series Database
 */
class get_camera_test : public CPPUNIT_NS::TestFixture
{
CPPUNIT_TEST_SUITE(get_camera_test);
CPPUNIT_TEST(extracts_camera_from_camera_set);
CPPUNIT_TEST(extracts_invalid_camera_set);
CPPUNIT_TEST(extracts_valid_extrinsic);
CPPUNIT_TEST(extracts_valid_extrinsic1);
CPPUNIT_TEST(extracts_invalid_extrinsic);
CPPUNIT_TEST(extracts_camera_out_of_bound_index);
CPPUNIT_TEST(extracts_extrinsic_out_of_bound_index);
CPPUNIT_TEST_SUITE_END();

public:

    // interface
    void setUp() override;
    void tearDown() override;

    static void extracts_camera_from_camera_set();
    static void extracts_invalid_camera_set();
    static void extracts_valid_extrinsic();
    static void extracts_valid_extrinsic1();
    static void extracts_camera_out_of_bound_index();
    static void extracts_extrinsic_out_of_bound_index();
    static void extracts_invalid_extrinsic();
};

} // namespace sight::module::data::ut
