/************************************************************************
 *
 * Copyright (C) 2009-2024 IRCAD France
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
CPPUNIT_TEST(add_config_test);
CPPUNIT_TEST(parameters_config_test);
CPPUNIT_TEST(start_stop_test);
CPPUNIT_TEST(auto_connect_test);
CPPUNIT_TEST(connection_test);
CPPUNIT_TEST(start_stop_connection_test);
CPPUNIT_TEST(optional_key_test);
CPPUNIT_TEST(key_group_test);
CPPUNIT_TEST(concurrent_access_to_config_test);
CPPUNIT_TEST(parameter_replace_test);
CPPUNIT_TEST(object_config_test);
CPPUNIT_TEST(properties_test);
CPPUNIT_TEST_SUITE_END();

public:

    // interface
    void setUp() override;
    void tearDown() override;

    static void add_config_test();
    static void parameters_config_test();
    void start_stop_test();
    void auto_connect_test();
    void connection_test();
    void start_stop_connection_test();
    void optional_key_test();
    void key_group_test();
    static void concurrent_access_to_config_test();
    void parameter_replace_test();
    void object_config_test();
    void properties_test();

private:

    static service::config_t build_config();

    static app::config_manager::sptr launch_app_config_mgr(
        const std::string& _name,
        bool _auto_prefix = false
    );

    app::config_manager::sptr m_app_config_mgr;
};

} // namespace sight::app::ut
