/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2018.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#pragma once

#include <cppunit/extensions/HelperMacros.h>

#include <OgreRoot.h>

namespace fwRenderOgre
{
namespace ut
{

class TextTest : public CPPUNIT_NS::TestFixture
{

CPPUNIT_TEST_SUITE( TextTest );
CPPUNIT_TEST( factoryTest );
CPPUNIT_TEST_SUITE_END();

public:

    TextTest();
    virtual ~TextTest();

    void setUp() override;
    void tearDown() override;

    void factoryTest();

private:
    /// Ogre root to setup the scene and instantiate 'Text' objects.
    ::Ogre::Root* m_ogreRoot;

};

} //namespace ut
} //namespace fwRenderOgre
