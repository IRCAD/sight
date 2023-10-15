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
CPPUNIT_TEST(isColorTechnique);
CPPUNIT_TEST(isPeelTechnique);
CPPUNIT_TEST(isGeometricTechnique);
CPPUNIT_TEST(isDepthOnlyTechnique);
CPPUNIT_TEST(getPermutation);
CPPUNIT_TEST(getR2VBGeometryProgramName);
CPPUNIT_TEST(setPermutationInProgramName);
CPPUNIT_TEST(setTechniqueInProgramName);
CPPUNIT_TEST(createObjectFromShaderParameter);
CPPUNIT_TEST_SUITE_END();

public:

    shading_test();

    // interface
    void setUp() override;
    void tearDown() override;

    void isColorTechnique();
    void isPeelTechnique();
    void isGeometricTechnique();
    void isDepthOnlyTechnique();
    static void getPermutation();
    static void getR2VBGeometryProgramName();
    static void setPermutationInProgramName();
    static void setTechniqueInProgramName();
    static void createObjectFromShaderParameter();

private:

    /// Dummy material created at runtime
    Ogre::MaterialPtr m_material;
};

} // namespace sight::viz::scene3d::helper::ut
