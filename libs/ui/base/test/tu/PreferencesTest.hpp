/************************************************************************
 *
 * Copyright (C) 2016-2022 IRCAD France
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

#include <core/runtime/EConfigurationElement.hpp>
#include <core/runtime/profile/Profile.hpp>

#include <cppunit/extensions/HelperMacros.h>

namespace sight::ui::base
{

namespace ut
{

/**
 * @brief Preferences unit test.
 */
class PreferencesTest : public CPPUNIT_NS::TestFixture
{
CPPUNIT_TEST_SUITE(PreferencesTest);
CPPUNIT_TEST(runtimeTest);
CPPUNIT_TEST(simpleTest);
CPPUNIT_TEST(delimeterTest);
CPPUNIT_TEST(parsedGetTest);
CPPUNIT_TEST(encryptedTest);
CPPUNIT_TEST(forcedEncryptionTest);
CPPUNIT_TEST_SUITE_END();

public:

    // interface
    void setUp();
    void tearDown();

    void runtimeTest();
    void simpleTest();
    void delimeterTest();
    void parsedGetTest();
    void encryptedTest();
    void forcedEncryptionTest();

private:

    core::runtime::Profile::sptr m_profile;
    std::filesystem::path m_preferencesPath;
    std::filesystem::path m_encryptedPath;
};

} //namespace ut

} //namespace sight::ui::base
