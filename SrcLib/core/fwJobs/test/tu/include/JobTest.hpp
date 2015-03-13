/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2015.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __FWJOBS_UT_JOBTEST_HPP__
#define __FWJOBS_UT_JOBTEST_HPP__

#include <cppunit/extensions/HelperMacros.h>

namespace fwJobs
{
namespace ut
{

class JobTest : public CPPUNIT_NS::TestFixture
{
CPPUNIT_TEST_SUITE( JobTest );
CPPUNIT_TEST( APIAndStateTest );
CPPUNIT_TEST( GenericCallbackTest );
CPPUNIT_TEST( ObserverTest );
CPPUNIT_TEST( LogTest );
CPPUNIT_TEST( AggregationTest );
CPPUNIT_TEST_SUITE_END();

public:
    // interface
    void setUp();
    void tearDown();

    void APIAndStateTest();
    void GenericCallbackTest();
    void LogTest();
    void AggregationTest();
    void ObserverTest();

};

} //namespace ut
} //namespace fwJobs
#endif //__FWJOBS_UT_JOBTEST_HPP__
