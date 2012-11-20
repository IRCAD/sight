/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2012.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _FWSERVICES_TEST_TU_SLOTSSIGNALSTEST_HPP_
#define _FWSERVICES_TEST_TU_SLOTSSIGNALSTEST_HPP_

#include <cppunit/extensions/HelperMacros.h>

namespace fwServices
{
namespace ut
{

class SlotsSignalsTest : public CPPUNIT_NS::TestFixture
{
    CPPUNIT_TEST_SUITE( SlotsSignalsTest );
    CPPUNIT_TEST( basicTest );
    CPPUNIT_TEST( comObjectServiceTest );
    CPPUNIT_TEST( comServiceToServiceTest );
    CPPUNIT_TEST_SUITE_END();


public:

    void setUp();
    void tearDown();

    void basicTest();
    void comObjectServiceTest();
    void comServiceToServiceTest();

};

} //namespace ut
} //namespace fwServices

#endif // _FWSERVICES_TEST_TU_SLOTSSIGNALSTEST_HPP_
