/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _FWSERVICES_TEST_TU_OBJECTSERVICETEST_HPP_
#define _FWSERVICES_TEST_TU_OBJECTSERVICETEST_HPP_

#include <cppunit/extensions/HelperMacros.h>
#include <fwRuntime/EConfigurationElement.hpp>

namespace fwServices
{
namespace ut
{

/**
 * @brief
 */
class ObjectServiceTest : public CPPUNIT_NS::TestFixture
{
    CPPUNIT_TEST_SUITE( ObjectServiceTest );
    CPPUNIT_TEST( swapTest );
    CPPUNIT_TEST( registerTest );
    CPPUNIT_TEST_SUITE_END();


public:
    // interface
    void setUp();
    void tearDown();

    void swapTest();
    void registerTest();
};

} //namespace ut
} //namespace fwServices

#endif //_FWSERVICES_TEST_TU_OBJECTSERVICETEST_HPP_
