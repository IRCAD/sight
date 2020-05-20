/************************************************************************
 *
 * Copyright (C) 2009-2015 IRCAD France
 * Copyright (C) 2012-2015 IHU Strasbourg
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
CPPUNIT_TEST( blockTest );
CPPUNIT_TEST( argumentLossTest );
CPPUNIT_TEST( asyncEmitTest );
CPPUNIT_TEST( asyncArgumentLossTest );

CPPUNIT_TEST_SUITE_END();

public:
    // interface
    void setUp();
    void tearDown();

    void buildTest ();
    void connectTest ();
    void emitTest ();
    void autoSlotDisconnectTest();
    void blockTest();
    void argumentLossTest();
    void asyncEmitTest ();
    void asyncArgumentLossTest();
};

} //namespace ut
} //namespace fwCom
#endif //__FWCOM_UT_SIGNALTEST_HPP__

