/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2013.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __IOMIDAS_TEST_TU_IOMIDASTEST_HPP__
#define __IOMIDAS_TEST_TU_IOMIDASTEST_HPP__

#include <cppunit/extensions/HelperMacros.h>

#include <fwServices/macros.hpp>

#include <midasIO/IConfiguration.hpp>

namespace ioMidas
{
namespace ut
{

class IoMidasTest : public CPPUNIT_NS::TestFixture
{
    CPPUNIT_TEST_SUITE( IoMidasTest );
    CPPUNIT_TEST( checkConfigurationTest );
    CPPUNIT_TEST( checkGetTest);
    CPPUNIT_TEST( checkCreateConfigurationTest );
    CPPUNIT_TEST( checkStatFieldTest );
    CPPUNIT_TEST( checkGetMessageTest );
    CPPUNIT_TEST( checkGetDataTest);
    CPPUNIT_TEST_SUITE_END();

public:
    // Interface
    void setUp();
    void tearDown();

    void checkConfigurationTest();
    void checkGetTest();
    void checkCreateConfigurationTest();
    void checkConnectionTest();
    void checkStatFieldTest();
    void checkGetMessageTest();
    void checkSetMessageTest();
    void checkGetDataTest();

};

} // namespace ut
} // namespace ioMidas

#endif // __IOMIDAS_TEST_TU_IOMIDASTEST_HPP__
