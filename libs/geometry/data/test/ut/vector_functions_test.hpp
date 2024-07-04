/************************************************************************
 *
 * Copyright (C) 2009-2024 IRCAD France
 * Copyright (C) 2012-2015 IHU Strasbourg
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

// cspell: ignore orthogonalize

namespace sight::geometry::data::ut
{

class vector_functions_test : public CPPUNIT_NS::TestFixture
{
private:

    CPPUNIT_TEST_SUITE(vector_functions_test);
    CPPUNIT_TEST(check_normalize);
    CPPUNIT_TEST(check_dot);
    CPPUNIT_TEST(check_cross);
    CPPUNIT_TEST(check_negate);
    CPPUNIT_TEST(check_vec_length);
    CPPUNIT_TEST(check_operators);
    CPPUNIT_TEST(check_orthogonalize);
    CPPUNIT_TEST_SUITE_END();

protected:

public:

    // interface
    void setUp() override;
    void tearDown() override;

    static void check_dot();
    static void check_cross();
    static void check_normalize();
    static void check_negate();
    static void check_vec_length();
    static void check_operators();
    static void check_orthogonalize();
};

} // namespace sight::geometry::data::ut
