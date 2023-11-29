/************************************************************************
 *
 * Copyright (C) 2009-2023 IRCAD France
 * Copyright (C) 2012-2020 IHU Strasbourg
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

#include "test_service.hpp"

#include <cppunit/extensions/HelperMacros.h>

namespace sight::service::ut
{

/**
 * @brief Test many methods to create service (with uuid, with configuration,...).
 * And test service methods (start, stop and update).
 */
class service_test : public CPPUNIT_NS::TestFixture
{
CPPUNIT_TEST_SUITE(service_test);
CPPUNIT_TEST(test_service_configuration);
CPPUNIT_TEST(test_service_creation_with_multiple_data);
CPPUNIT_TEST(test_service_creation_with_template_methods);
CPPUNIT_TEST(test_service_creation_with_uuid);
CPPUNIT_TEST(test_start_stop_update);
CPPUNIT_TEST(test_start_stop_update_exceptions);
CPPUNIT_TEST(test_communication);
CPPUNIT_TEST(test_with_in_and_out);
CPPUNIT_TEST_SUITE_END();

public:

    // interface
    void setUp() override;
    void tearDown() override;

    /// Test to configure a service
    static void test_service_configuration();

    /// Test add and remove service with multiple associated data
    static void test_service_creation_with_multiple_data();

    /// Test add and remove service using template methods
    static void test_service_creation_with_template_methods();

    /// Test add and remove service with UUID
    static void test_service_creation_with_uuid();

    /// test start/stop/update service
    static void test_start_stop_update();

    /// test exceptions handling when starting/stopping/updating services
    static void test_start_stop_update_exceptions();

    /// test sending/receiving message
    static void test_communication();

    /// test service with an input and an output
    static void test_with_in_and_out();

private:

    static void start_stop_update_exceptions(service::ut::test_service::sptr _service);
};

} // namespace sight::service::ut
