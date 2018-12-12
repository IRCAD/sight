/************************************************************************
 *
 * Copyright (C) 2009-2018 IRCAD France
 * Copyright (C) 2012-2018 IHU Strasbourg
 *
 * This file is part of Sight.
 *
 * Sight is free software: you can redistribute it and/or modify it under
 * the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Sight is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with Sight. If not, see <https://www.gnu.org/licenses/>.
 *
 ***********************************************************************/

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
