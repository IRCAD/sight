/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __GUIQT_UT_GUIQTTEST_HPP__
#define __GUIQT_UT_GUIQTTEST_HPP__

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
CPPUNIT_TEST( testTuto01 );
CPPUNIT_TEST( testTuto02 );
CPPUNIT_TEST_SUITE_END();

public:
    // interface
    void setUp();
    void tearDown();

    void testDefaultFrame();
    void testTuto01();
    void testTuto02();

};

} //namespace ut
} //namespace guiQt

#endif //__GUIQT_UT_GUIQTTEST_HPP__
