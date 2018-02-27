/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2018.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#pragma once

#include <fwRuntime/EConfigurationElement.hpp>

#include <cppunit/extensions/HelperMacros.h>

namespace fwServices
{
namespace ut
{

/**
 * @brief Test .
 */
class ServiceConfigTest : public CPPUNIT_NS::TestFixture
{
CPPUNIT_TEST_SUITE( ServiceConfigTest );
CPPUNIT_TEST( serviceConfigTest );
CPPUNIT_TEST( concurentAccessToServiceConfigTest );
CPPUNIT_TEST( getAllConfigsTest );
CPPUNIT_TEST_SUITE_END();

public:
    // interface
    void setUp();
    void tearDown();

    void serviceConfigTest();
    void concurentAccessToServiceConfigTest();
    void getAllConfigsTest();

private:
    ::fwRuntime::ConfigurationElement::sptr buildConfig();
};

} //namespace ut
} //namespace fwServices
