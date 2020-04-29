/************************************************************************
 *
 * Copyright (C) 2016-2020 IRCAD France
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

#include <fwRuntime/EConfigurationElement.hpp>
#include <fwRuntime/profile/Profile.hpp>

#include <cppunit/extensions/HelperMacros.h>

namespace fwPreferences
{
namespace ut
{

/**
 * @brief Preferences unit test.
 */
class PreferencesTest : public CPPUNIT_NS::TestFixture
{
CPPUNIT_TEST_SUITE( PreferencesTest );
CPPUNIT_TEST( runtimeTest );
CPPUNIT_TEST( helperTest );
CPPUNIT_TEST( passwordTest );
CPPUNIT_TEST( cleanup );
CPPUNIT_TEST_SUITE_END();

public:
    // interface
    void setUp();
    void tearDown();

    void runtimeTest();
    void helperTest();
    void passwordTest();
    void cleanup();

private:
    ::fwRuntime::profile::Profile::sptr m_profile;
    std::filesystem::path m_preferencesPath;
};

} //namespace ut
} //namespace fwPreferences
