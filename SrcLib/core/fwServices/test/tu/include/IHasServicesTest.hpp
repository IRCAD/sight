/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __FWSERVICES_UT_IHASSERVICESTEST_HPP__
#define __FWSERVICES_UT_IHASSERVICESTEST_HPP__

#include <cppunit/extensions/HelperMacros.h>

namespace fwServices
{
namespace ut
{

/**
 * @brief Test IHasServices interface
 */
class IHasServicesTest : public CPPUNIT_NS::TestFixture
{
CPPUNIT_TEST_SUITE( IHasServicesTest );
CPPUNIT_TEST( testRegistration );
CPPUNIT_TEST( testConnection );
CPPUNIT_TEST_SUITE_END();

public:
    // interface
    void setUp();
    void tearDown();

    /// Test register/unregister methods
    void testRegistration();
    /// Test connection with data
    void testConnection();
};

} //namespace ut
} //namespace fwServices

#endif // __FWSERVICES_UT_IHASSERVICESTEST_HPP__

