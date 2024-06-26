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

#include <data/matrix4.hpp>

#include <cppunit/extensions/HelperMacros.h>

namespace sight::geometry::data::ut
{

/**
 * @brief Test many methods to create image.
 */
class matrix4_test : public CPPUNIT_NS::TestFixture
{
CPPUNIT_TEST_SUITE(matrix4_test);
CPPUNIT_TEST(identity_matrix_test);
CPPUNIT_TEST(matrix_test);
CPPUNIT_TEST(glm_getter_setter_test);
CPPUNIT_TEST(orthogonality_test);
CPPUNIT_TEST_SUITE_END();

public:

    // interface
    void setUp() override;
    void tearDown() override;

    static void identity_matrix_test();
    static void matrix_test();
    static void glm_getter_setter_test();
    static void orthogonality_test();
};

} // namespace sight::geometry::data::ut
