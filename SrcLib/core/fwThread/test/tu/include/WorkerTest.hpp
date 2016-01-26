/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2015.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __FWTHREAD_UT_WORKERTEST_HPP__
#define __FWTHREAD_UT_WORKERTEST_HPP__

#include <cppunit/extensions/HelperMacros.h>

namespace fwThread
{
namespace ut
{

class WorkerTest : public CPPUNIT_NS::TestFixture
{
CPPUNIT_TEST_SUITE( WorkerTest );
CPPUNIT_TEST( basicTest );
CPPUNIT_TEST( timerTest );
CPPUNIT_TEST_SUITE_END();

public:
    // interface
    void setUp();
    void tearDown();

    void basicTest();
    void timerTest();

};

} //namespace ut
} //namespace fwThread
#endif //__FWTHREAD_UT_WORKERTEST_HPP__
