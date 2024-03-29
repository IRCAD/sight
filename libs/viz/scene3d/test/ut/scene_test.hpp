/************************************************************************
 *
 * Copyright (C) 2015-2023 IRCAD France
 * Copyright (C) 2015-2021 IHU Strasbourg
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

#include <OGRE/OgreRoot.h>

namespace sight::viz::scene3d::helper::ut
{

class scene_test : public CPPUNIT_NS::TestFixture
{
CPPUNIT_TEST_SUITE(scene_test);
CPPUNIT_TEST(get_node_by_id);
CPPUNIT_TEST(generate_color);
CPPUNIT_TEST(get_length);
CPPUNIT_TEST(spacing_as_vector3);
CPPUNIT_TEST_SUITE_END();

public:

    // interface
    void setUp() override;
    void tearDown() override;

    static void get_node_by_id();
    static void generate_color();
    static void get_length();
    static void spacing_as_vector3();
};

} // namespace sight::viz::scene3d::helper::ut
