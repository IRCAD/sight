/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2018.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#pragma once

#include <cppunit/extensions/HelperMacros.h>

namespace guiQt
{
namespace ut
{

/**
 * @brief Test many methods to create mesh.
 */
class GuiQtTest : public CPPUNIT_NS::TestFixture
{
CPPUNIT_TEST_SUITE( GuiQtTest );
CPPUNIT_TEST( testDefaultFrame );
CPPUNIT_TEST_SUITE_END();

public:
    // interface
    void setUp();
    void tearDown();

    void testDefaultFrame();

};

} //namespace ut
} //namespace guiQt
