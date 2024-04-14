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

namespace sight::viz::scene3d::ut
{

class utils_test : public CPPUNIT_NS::TestFixture
{
CPPUNIT_TEST_SUITE(utils_test);
CPPUNIT_TEST(from_ogre_color);
CPPUNIT_TEST(convert_ogre_matrix_to_t_m_3d);
CPPUNIT_TEST(world_to_slice_test);
CPPUNIT_TEST_SUITE_END();

public:

    // interface
    void setUp() override;
    void tearDown() override;

    static void from_ogre_color();
    static void convert_ogre_matrix_to_t_m_3d();
    static void world_to_slice_test();
};

} // namespace sight::viz::scene3d::ut
