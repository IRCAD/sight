/************************************************************************
 *
 * Copyright (C) 2009-2023 IRCAD France
 * Copyright (C) 2012-2021 IHU Strasbourg
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

#include <app/config_manager.hpp>

#include <service/op.hpp>

#include <cppunit/extensions/HelperMacros.h>

namespace sight::app::ut
{

/**
 * @brief Test .
 */
class config_test : public CPPUNIT_NS::TestFixture
{
CPPUNIT_TEST_SUITE(config_test);
CPPUNIT_TEST(addConfigTest);
CPPUNIT_TEST(parametersConfigTest);
CPPUNIT_TEST(startStopTest);
CPPUNIT_TEST(autoConnectTest);
CPPUNIT_TEST(connectionTest);
CPPUNIT_TEST(startStopConnectionTest);
CPPUNIT_TEST(optionalKeyTest);
CPPUNIT_TEST(keyGroupTest);
CPPUNIT_TEST(concurrent_access_to_config_test);
CPPUNIT_TEST(parameterReplaceTest);
CPPUNIT_TEST(objectConfigTest);
CPPUNIT_TEST_SUITE_END();

public:

    // interface
    void setUp() override;
    void tearDown() override;

    static void addConfigTest();
    static void parametersConfigTest();
    void startStopTest();
    void autoConnectTest();
    void connectionTest();
    void startStopConnectionTest();
    void optionalKeyTest();
    void keyGroupTest();
    static void concurrent_access_to_config_test();
    void parameterReplaceTest();
    void objectConfigTest();

private:

    static service::config_t buildConfig();

    static app::config_manager::sptr launchAppConfigMgr(
        const std::string& _name,
        bool _auto_prefix = false
    );

    app::config_manager::sptr m_appConfigMgr;
};

} // namespace sight::app::ut
