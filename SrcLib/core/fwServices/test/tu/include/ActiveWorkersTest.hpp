/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __FWSERVICES_UT_ACTIVEWORKERSTEST_HPP__
#define __FWSERVICES_UT_ACTIVEWORKERSTEST_HPP__

#include <cppunit/extensions/HelperMacros.h>

namespace fwServices
{
namespace ut
{

/// Test ActiveWorkers API
class ActiveWorkersTest : public CPPUNIT_NS::TestFixture
{
CPPUNIT_TEST_SUITE( ActiveWorkersTest );
CPPUNIT_TEST( basicTest );
CPPUNIT_TEST_SUITE_END();


public:

    void setUp();
    void tearDown();

    /// Test using ActiveWorkers API
    void basicTest();

};

} //namespace ut
} //namespace fwServices

#endif //__FWSERVICES_UT_ACTIVEWORKERSTEST_HPP__
