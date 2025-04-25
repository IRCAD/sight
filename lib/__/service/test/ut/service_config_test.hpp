/************************************************************************
 *
 * Copyright (C) 2009-2023 IRCAD France
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

#include <service/op.hpp>

#include <cppunit/extensions/HelperMacros.h>

namespace sight::service::ut
{

/**
 * @brief Test .
 */
class service_config_test : public CPPUNIT_NS::TestFixture
{
CPPUNIT_TEST_SUITE(service_config_test);
CPPUNIT_TEST(config_test);
CPPUNIT_TEST(concurrent_access_to_service_config_test);
CPPUNIT_TEST(get_all_configs_test);
CPPUNIT_TEST_SUITE_END();

public:

    // interface
    void setUp() override;
    void tearDown() override;

    static void config_test();
    static void concurrent_access_to_service_config_test();
    static void get_all_configs_test();

private:

    static service::config_t build_config();
};

} // namespace sight::service::ut
