/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __FWSERVICES_UT_APPCONFIG2TEST_HPP__
#define __FWSERVICES_UT_APPCONFIG2TEST_HPP__

#include <cppunit/extensions/HelperMacros.h>
#include <fwRuntime/EConfigurationElement.hpp>

#include <fwServices/AppConfigManager2.hpp>

namespace fwServices
{
namespace ut
{

/**
 * @brief Test .
 */
class AppConfig2Test : public CPPUNIT_NS::TestFixture
{
CPPUNIT_TEST_SUITE( AppConfig2Test );
CPPUNIT_TEST( parametersConfigTest );
CPPUNIT_TEST( startStopTest );
CPPUNIT_TEST( autoConnectTest );
CPPUNIT_TEST( connectionTest );
CPPUNIT_TEST( optionalKeyTest );
CPPUNIT_TEST( keyGroupTest );
CPPUNIT_TEST( concurentAccessToAppConfig2Test );
CPPUNIT_TEST( parameterReplaceTest );
CPPUNIT_TEST_SUITE_END();


public:
    // interface
    void setUp();
    void tearDown();

    void parametersConfigTest();
    void startStopTest();
    void autoConnectTest();
    void connectionTest();
    void optionalKeyTest();
    void keyGroupTest();
    void concurentAccessToAppConfig2Test();
    void parameterReplaceTest();

private:
    ::fwRuntime::ConfigurationElement::sptr buildConfig();
    ::fwRuntime::ConfigurationElement::sptr buildParametersConfig();
    ::fwRuntime::ConfigurationElement::sptr buildStartStopConfig();
    ::fwRuntime::ConfigurationElement::sptr buildAutoConnectTestConfig();
    ::fwRuntime::ConfigurationElement::sptr buildConnectionConfig();
    ::fwRuntime::ConfigurationElement::sptr buildOptionalKeyConfig();
    ::fwRuntime::ConfigurationElement::sptr buildKeyGroupConfig();
    ::fwRuntime::ConfigurationElement::sptr buildParameterReplaceConfig();


    ::fwServices::AppConfigManager2::sptr launchAppConfigMgr(const std::string &name,
                                                             const ::fwRuntime::ConfigurationElement::csptr& config,
                                                             bool autoPrefix = false);

    ::fwServices::AppConfigManager2::sptr m_appConfigMgr;
};

} //namespace ut
} //namespace fwServices

#endif // __FWSERVICES_UT_APPCONFIG2TEST_HPP__
