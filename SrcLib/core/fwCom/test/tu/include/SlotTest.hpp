/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2012.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __FWCOM_UT_UTILTEST_HPP__
#define __FWCOM_UT_UTILTEST_HPP__

#include <cppunit/extensions/HelperMacros.h>

namespace fwCom
{
namespace ut
{

class SlotTest : public CPPUNIT_NS::TestFixture
{
    CPPUNIT_TEST_SUITE( SlotTest );

    CPPUNIT_TEST( buildTest );
    CPPUNIT_TEST( runTest );
    CPPUNIT_TEST( callTest );
    CPPUNIT_TEST( asyncTest );
    CPPUNIT_TEST( slotBaseTest );
    CPPUNIT_TEST( exceptionTest );

    CPPUNIT_TEST_SUITE_END();

public:
    // interface
    void setUp();
    void tearDown();

    void buildTest ();
    void runTest ();
    void callTest ();
    void asyncTest ();
    void slotBaseTest ();
    void exceptionTest ();

};

} //namespace ut
} //namespace fwCom
#endif //__FWCOM_UT_UTILTEST_HPP__
