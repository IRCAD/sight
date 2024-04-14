/************************************************************************
 *
 * Copyright (C) 2014-2023 IRCAD France
 * Copyright (C) 2014-2021 IHU Strasbourg
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

#include <OGRE/OgreMaterial.h>

namespace sight::viz::scene3d::helper::ut
{

class shading_test : public CPPUNIT_NS::TestFixture
{
CPPUNIT_TEST_SUITE(shading_test);
CPPUNIT_TEST(is_color_technique);
CPPUNIT_TEST(is_peel_technique);
CPPUNIT_TEST(is_geometric_technique);
CPPUNIT_TEST(is_depth_only_technique);
CPPUNIT_TEST(get_permutation);
CPPUNIT_TEST(get_r2_vb_geometry_program_name);
CPPUNIT_TEST(set_permutation_in_program_name);
CPPUNIT_TEST(set_technique_in_program_name);
CPPUNIT_TEST(create_object_from_shader_parameter);
CPPUNIT_TEST_SUITE_END();

public:

    shading_test();

    // interface
    void setUp() override;
    void tearDown() override;

    void is_color_technique();
    void is_peel_technique();
    void is_geometric_technique();
    void is_depth_only_technique();
    static void get_permutation();
    static void get_r2_vb_geometry_program_name();
    static void set_permutation_in_program_name();
    static void set_technique_in_program_name();
    static void create_object_from_shader_parameter();

private:

    /// Dummy material created at runtime
    Ogre::MaterialPtr m_material;
};

} // namespace sight::viz::scene3d::helper::ut
