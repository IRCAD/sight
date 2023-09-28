/************************************************************************
 *
 * Copyright (C) 2021-2023 IRCAD France
 * Copyright (C) 2017 IHU Strasbourg
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

#include <cppunit/extensions/HelperMacros.h>

namespace sight::service::ut
{

/**
 * @brief Test has_services interface
 */
class has_servicesTest : public CPPUNIT_NS::TestFixture
{
CPPUNIT_TEST_SUITE(has_servicesTest);
CPPUNIT_TEST(testRegistration);
CPPUNIT_TEST(testConnection);
CPPUNIT_TEST(testOptionalInputs);
CPPUNIT_TEST_SUITE_END();

public:

    // interface
    void setUp() override;
    void tearDown() override;

    /// Test register/unregister methods
    static void testRegistration();
    /// Test connection with data
    static void testConnection();
    /// Test registering optional inputs
    static void testOptionalInputs();
};

} // namespace sight::service::ut
