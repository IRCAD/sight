/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2018.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#pragma once

#include <cppunit/extensions/HelperMacros.h>

namespace fwRenderOgre
{
namespace ut
{

class UtilsTest : public CPPUNIT_NS::TestFixture
{
CPPUNIT_TEST_SUITE( UtilsTest );
CPPUNIT_TEST( convertOgreColorToFwColor );
CPPUNIT_TEST( convertOgreMatrixToTM3D );
CPPUNIT_TEST_SUITE_END();

public:
    // interface
    void setUp();
    void tearDown();

    void convertOgreColorToFwColor();
    void convertOgreMatrixToTM3D();
};

} //namespace ut
} //namespace fwRenderOgre
