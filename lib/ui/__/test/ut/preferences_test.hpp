/************************************************************************
 *
 * Copyright (C) 2016-2025 IRCAD France
 * Copyright (C) 2016-2020 IHU Strasbourg
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

#include <core/runtime/profile/profile.hpp>

#include <cppunit/extensions/HelperMacros.h>

namespace sight::ui::ut
{

/**
 * @brief preferences unit test.
 */
class preferences_test : public CPPUNIT_NS::TestFixture
{
CPPUNIT_TEST_SUITE(preferences_test);
CPPUNIT_TEST(runtime_test);
CPPUNIT_TEST(simple_test);
CPPUNIT_TEST(delimeter_test);
CPPUNIT_TEST(parsed_get_test);
CPPUNIT_TEST(encrypted_test);
CPPUNIT_TEST(forced_encryption_test);
CPPUNIT_TEST(corrupted_test);
CPPUNIT_TEST_SUITE_END();

public:

    // interface
    void setUp() override;
    void tearDown() override;

    void runtime_test();
    void simple_test();
    static void delimeter_test();
    static void parsed_get_test();
    void encrypted_test();
    void forced_encryption_test();
    void corrupted_test();

private:

    core::runtime::profile::sptr m_profile;
    std::filesystem::path m_preferences_path;
    std::filesystem::path m_encrypted_path;
};

} // namespace sight::ui::ut
