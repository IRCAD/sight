/************************************************************************
 *
 * Copyright (C) 2009-2023 IRCAD France
 * Copyright (C) 2012-2021 IHU Strasbourg
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

#include <limits>

namespace sight::data::ut
{

class mesh_test : public CPPUNIT_NS::TestFixture
{
private:

    CPPUNIT_TEST_SUITE(mesh_test);
    CPPUNIT_TEST(insertion);
    CPPUNIT_TEST(copy);
    CPPUNIT_TEST(allocation_test);
    CPPUNIT_TEST(iterator_test);
    CPPUNIT_TEST(iterator_copy_test);
    CPPUNIT_TEST(benchmark_iterator);
    CPPUNIT_TEST(equality_test);
    CPPUNIT_TEST_SUITE_END();

    static constexpr float EPSILON = std::numeric_limits<float>::epsilon();

public:

    // interface
    void setUp() override;
    void tearDown() override;

    static void insertion();
    static void copy();
    static void allocation_test();
    static void iterator_test();
    static void iterator_copy_test();
    static void benchmark_iterator();
    static void equality_test();
};

} // namespace sight::data::ut
