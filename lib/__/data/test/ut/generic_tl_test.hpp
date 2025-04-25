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

#include <cppunit/extensions/HelperMacros.h>

namespace sight::data::ut
{

class generic_tl_test : public CPPUNIT_NS::TestFixture
{
public:

    CPPUNIT_TEST_SUITE(generic_tl_test);
    CPPUNIT_TEST(push_pop_test);
    CPPUNIT_TEST(push_class_test);
    CPPUNIT_TEST(copy_test);
    CPPUNIT_TEST(iterator_test);
    CPPUNIT_TEST(object_valid);
    CPPUNIT_TEST(equality_test);
    CPPUNIT_TEST_SUITE_END();

public:

    // interface
    void setUp() override;
    void tearDown() override;

    static void push_pop_test();
    static void push_class_test();
    static void copy_test();
    static void iterator_test();
    static void object_valid();
    static void equality_test();
};

} // namespace sight::data::ut
