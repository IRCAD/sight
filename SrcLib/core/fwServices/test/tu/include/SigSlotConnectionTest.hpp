/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __FWSERVICES_UT_SIGSLOTCONNECTIONTEST_HPP__
#define __FWSERVICES_UT_SIGSLOTCONNECTIONTEST_HPP__

#include <cppunit/extensions/HelperMacros.h>

namespace fwServices
{
namespace ut
{

class SigSlotConnectionTest : public CPPUNIT_NS::TestFixture
{
CPPUNIT_TEST_SUITE( SigSlotConnectionTest );
CPPUNIT_TEST( basicTest );
CPPUNIT_TEST_SUITE_END();


public:

    void setUp();
    void tearDown();

    void basicTest();
};

} //namespace ut
} //namespace fwServices

#endif // __FWSERVICES_UT_SIGSLOTCONNECTIONTEST_HPP__
