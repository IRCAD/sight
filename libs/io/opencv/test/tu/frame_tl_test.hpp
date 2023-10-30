/************************************************************************
 *
 * Copyright (C) 2021-2023 IRCAD France
 * Copyright (C) 2017 IHU Strasbourg
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

namespace sight::io::opencv::ut
{

/**
 * @brief Tests the io::opencv::frame_tl functions
 */
class frame_tl_test : public CPPUNIT_NS::TestFixture
{
public:

    CPPUNIT_TEST_SUITE(frame_tl_test);
    CPPUNIT_TEST(move_to_cv);
    CPPUNIT_TEST(copy_from_cv);
    CPPUNIT_TEST(copy_to_cv);
    CPPUNIT_TEST_SUITE_END();

public:

    // interface
    void setUp() override;
    void tearDown() override;

    static void move_to_cv();
    static void copy_from_cv();
    static void copy_to_cv();
};

} // namespace sight::io::opencv::ut
