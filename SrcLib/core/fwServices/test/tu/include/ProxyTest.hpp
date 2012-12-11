/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2012.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _FWSERVICES_TEST_TU_PROXYTEST_HPP_
#define _FWSERVICES_TEST_TU_PROXYTEST_HPP_

#include <cppunit/extensions/HelperMacros.h>

namespace fwServices
{
namespace ut
{

/// Test ActiveWorkers API
class ProxyTest : public CPPUNIT_NS::TestFixture
{
    CPPUNIT_TEST_SUITE( ProxyTest );
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

#endif // _FWSERVICES_TEST_TU_PROXYTEST_HPP_
