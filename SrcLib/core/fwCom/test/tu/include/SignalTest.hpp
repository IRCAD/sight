/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2012.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __FWCOM_UT_SIGNALTEST_HPP__
#define __FWCOM_UT_SIGNALTEST_HPP__

#include <cppunit/extensions/HelperMacros.h>

namespace fwCom
{
namespace ut
{

class SignalTest : public CPPUNIT_NS::TestFixture
{
    CPPUNIT_TEST_SUITE( SignalTest );

    CPPUNIT_TEST( buildTest );
    CPPUNIT_TEST( connectTest );
    CPPUNIT_TEST( emitTest );
    CPPUNIT_TEST( autoSlotDisconnectTest );
    CPPUNIT_TEST( argumentLossTest );
    CPPUNIT_TEST( asyncEmitTest );

    CPPUNIT_TEST_SUITE_END();

public:
    // interface
    void setUp();
    void tearDown();

    void buildTest ();
    void connectTest ();
    void emitTest ();
    void autoSlotDisconnectTest();
    void argumentLossTest();
    void asyncEmitTest ();
};

} //namespace ut
} //namespace fwCom
#endif //__FWCOM_UT_SIGNALTEST_HPP__

