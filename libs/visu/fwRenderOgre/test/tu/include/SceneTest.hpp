/************************************************************************
 *
 * Copyright (C) 2015-2017 IRCAD France
 * Copyright (C) 2015-2017 IHU Strasbourg
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

#ifndef __FWRENDEROGRE_UT_SCENETEST_HPP__
#define __FWRENDEROGRE_UT_SCENETEST_HPP__

#include <cppunit/extensions/HelperMacros.h>

#include <OGRE/OgreRoot.h>

namespace fwRenderOgre
{
namespace helper
{

namespace ut
{

class SceneTest : public CPPUNIT_NS::TestFixture
{
CPPUNIT_TEST_SUITE( SceneTest );
CPPUNIT_TEST( getNodeById );
CPPUNIT_TEST_SUITE_END();

public:
    SceneTest();
    virtual ~SceneTest();
    // interface
    void setUp();
    void tearDown();

    void getNodeById();

private:
    /// Ogre root necessary to setup up everything properly
    ::Ogre::Root* m_ogreRoot;
};

} //namespace ut
} //namespace helper
} //namespace fwRenderOgre

#endif // __FWRENDEROGRE_UT_SCENETEST_HPP__
