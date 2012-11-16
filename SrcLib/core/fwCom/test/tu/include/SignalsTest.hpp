/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2012.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __FWCOM_UT_SIGNALSTEST_HPP__
#define __FWCOM_UT_SIGNALSTEST_HPP__

#include <cppunit/extensions/HelperMacros.h>

namespace fwCom
{
namespace ut
{

class SignalsTest : public CPPUNIT_NS::TestFixture
{
    CPPUNIT_TEST_SUITE( SignalsTest );

    CPPUNIT_TEST( buildTest );
    CPPUNIT_TEST( hasSignalsTest );

    CPPUNIT_TEST_SUITE_END();

public:

    void setUp();
    void tearDown();

    void buildTest();
    void hasSignalsTest();

};

} //namespace ut
} //namespace fwCom
#endif //__FWCOM_UT_SIGNALSTEST_HPP__
