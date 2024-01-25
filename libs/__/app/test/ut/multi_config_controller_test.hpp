/************************************************************************
 *
 * Copyright (C) 2023-2024 IRCAD France
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

#include <cppunit/extensions/HelperMacros.h>

namespace sight::app::ut
{

/**
 * @brief Test .
 */
class multi_config_controller_test : public CPPUNIT_NS::TestFixture
{
CPPUNIT_TEST_SUITE(multi_config_controller_test);
CPPUNIT_TEST(multi_config_test);
CPPUNIT_TEST(set_config_key_test);
CPPUNIT_TEST(default_loading_test);
CPPUNIT_TEST_SUITE_END();

public:

    // interface
    void setUp() override;
    void tearDown() override;

    // start the multiconfigController
    // set a config (slot) and check that the right service is started.
    // set the other config and test.
    // set the initial config again.
    static void multi_config_test();

    //Test if the setConfig slot with a bad key is ignored.
    // Starts the service and setConfig with the right key.
    // Calls setConfig with a bad key. Update and test that the service are still the same.
    static void set_config_key_test();

    //Test the default config loaded when update is call without any setConfig before.
    static void default_loading_test();
};

} // namespace sight::app::ut
