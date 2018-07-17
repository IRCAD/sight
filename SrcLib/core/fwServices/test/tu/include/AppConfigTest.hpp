/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2018.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#pragma once

#include <fwServices/AppConfigManager.hpp>

#include <fwRuntime/EConfigurationElement.hpp>

#include <cppunit/extensions/HelperMacros.h>

namespace fwServices
{
namespace ut
{

/**
 * @brief Test .
 */
class AppConfigTest : public CPPUNIT_NS::TestFixture
{
CPPUNIT_TEST_SUITE( AppConfigTest );
CPPUNIT_TEST( addConfigTest );
CPPUNIT_TEST( parametersConfigTest );
CPPUNIT_TEST( startStopTest );
CPPUNIT_TEST( autoConnectTest );
CPPUNIT_TEST( connectionTest );
CPPUNIT_TEST( optionalKeyTest );
CPPUNIT_TEST( keyGroupTest );
CPPUNIT_TEST( concurentAccessToAppConfigTest );
CPPUNIT_TEST( parameterReplaceTest );
CPPUNIT_TEST_SUITE_END();

public:
    // interface
    void setUp();
    void tearDown();

    void addConfigTest();
    void parametersConfigTest();
    void startStopTest();
    void autoConnectTest();
    void connectionTest();
    void optionalKeyTest();
    void keyGroupTest();
    void concurentAccessToAppConfigTest();
    void parameterReplaceTest();

private:
    ::fwRuntime::ConfigurationElement::sptr buildConfig();

    ::fwServices::AppConfigManager::sptr launchAppConfigMgr(const std::string& name,
                                                            bool autoPrefix = false);

    ::fwServices::AppConfigManager::sptr m_appConfigMgr;
};

} //namespace ut
} //namespace fwServices
