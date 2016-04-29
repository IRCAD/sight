/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2015-2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __FWTHREAD_UT_POOLTEST_HPP__
#define __FWTHREAD_UT_POOLTEST_HPP__

#include <cppunit/extensions/HelperMacros.h>

namespace fwThread
{
namespace ut
{

class PoolTest : public CPPUNIT_NS::TestFixture
{
CPPUNIT_TEST_SUITE( PoolTest );
CPPUNIT_TEST( basicTest );
CPPUNIT_TEST( defaultPoolTest );
CPPUNIT_TEST_SUITE_END();

public:
    // interface
    void setUp();
    void tearDown();

    void basicTest();
    void defaultPoolTest();

};

} //namespace ut
} //namespace fwThread
#endif //__FWTHREAD_UT_POOLTEST_HPP__
