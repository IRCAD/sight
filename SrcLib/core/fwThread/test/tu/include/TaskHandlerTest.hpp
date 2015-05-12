/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2015.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __FWTHREAD_UT_TASKHANDLERTEST_HPP__
#define __FWTHREAD_UT_TASKHANDLERTEST_HPP__

#include <cppunit/extensions/HelperMacros.h>

namespace fwThread
{
namespace ut
{

class TaskHandlerTest : public CPPUNIT_NS::TestFixture
{
CPPUNIT_TEST_SUITE( TaskHandlerTest );
CPPUNIT_TEST( basicTest );
CPPUNIT_TEST( exceptionTest );
CPPUNIT_TEST_SUITE_END();

public:
    // interface
    void setUp();
    void tearDown();

    void basicTest();
    void exceptionTest();

};

} //namespace ut
} //namespace fwThread
#endif //__FWTHREAD_UT_TASKHANDLERTEST_HPP__
