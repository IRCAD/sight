/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2015-2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

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
