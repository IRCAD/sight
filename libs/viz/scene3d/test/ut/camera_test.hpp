/************************************************************************
 *
 * Copyright (C) 2021-2023 IRCAD France
 * Copyright (C) 2021 IHU Strasbourg
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

#include <OgreRoot.h>

namespace sight::viz::scene3d::ut
{

class camera_test : public CPPUNIT_NS::TestFixture
{
CPPUNIT_TEST_SUITE(camera_test);
CPPUNIT_TEST(compute_projection_matrix);
CPPUNIT_TEST(convert_pixel_to_world_space);
CPPUNIT_TEST(convert_world_space_to_screen_space);
CPPUNIT_TEST_SUITE_END();

public:

    void setUp() override;
    void tearDown() override;

    static void compute_projection_matrix();
    static void convert_pixel_to_world_space();
    static void convert_world_space_to_screen_space();
};

} // namespace sight::viz::scene3d::ut
