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

class CameraTest : public CPPUNIT_NS::TestFixture
{

CPPUNIT_TEST_SUITE( CameraTest );
CPPUNIT_TEST( computeProjectionMatrix );
CPPUNIT_TEST_SUITE_END();

public:

    void setUp() override;
    void tearDown() override;

    void computeProjectionMatrix();

};

} //namespace ut
} //namespace fwRenderOgre
