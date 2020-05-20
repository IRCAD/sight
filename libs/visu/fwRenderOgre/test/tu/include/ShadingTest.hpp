/************************************************************************
 *
 * Copyright (C) 2014-2017 IRCAD France
 * Copyright (C) 2014-2017 IHU Strasbourg
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

#ifndef __FWRENDEROGRE_UT_SHADINGTEST_HPP__
#define __FWRENDEROGRE_UT_SHADINGTEST_HPP__

#include <cppunit/extensions/HelperMacros.h>

#include <OGRE/OgreMaterial.h>

namespace fwRenderOgre
{
namespace helper
{

namespace ut
{

class ShadingTest : public CPPUNIT_NS::TestFixture
{
CPPUNIT_TEST_SUITE( ShadingTest );
CPPUNIT_TEST( isColorTechnique );
CPPUNIT_TEST( isPeelTechnique );
CPPUNIT_TEST( isGeometricTechnique );
CPPUNIT_TEST( isDepthOnlyTechnique );
CPPUNIT_TEST( getPermutation );
CPPUNIT_TEST( getR2VBGeometryProgramName );
CPPUNIT_TEST( setPermutationInProgramName );
CPPUNIT_TEST( setTechniqueInProgramName );
CPPUNIT_TEST( createObjectFromShaderParameter );
CPPUNIT_TEST_SUITE_END();

public:
    // interface
    void setUp();
    void tearDown();

    void isColorTechnique();
    void isPeelTechnique();
    void isGeometricTechnique();
    void isDepthOnlyTechnique();
    void getPermutation();
    void getR2VBGeometryProgramName();
    void setPermutationInProgramName();
    void setTechniqueInProgramName();
    void createObjectFromShaderParameter();

private:
    /// Ogre root necessary to setup up everything properly
    ::Ogre::Root* m_ogreRoot;
    /// Dummy material created at runtime
    ::Ogre::MaterialPtr m_material;
};

} //namespace ut
} //namespace helper
} //namespace fwRenderOgre

#endif // __FWRENDEROGRE_UT_SHADINGTEST_HPP__
