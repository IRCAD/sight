/************************************************************************
 *
 * Copyright (C) 2016-2021 IRCAD France
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

#include "PreferencesTest.hpp"

#include <gui/preferences/helper.hpp>

#include <core/runtime/operations.hpp>
#include <core/runtime/Profile.hpp>
#include <core/tools/Os.hpp>
#include <core/tools/UUID.hpp>

#include <data/Composite.hpp>
#include <data/String.hpp>

#include <filesystem>

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION( gui::preferences::ut::PreferencesTest );

namespace sight::gui::preferences
{
namespace ut
{

//------------------------------------------------------------------------------

void PreferencesTest::setUp()
{
}

//------------------------------------------------------------------------------

void PreferencesTest::tearDown()
{
}

//------------------------------------------------------------------------------

void PreferencesTest::runtimeTest()
{
    core::runtime::init();

    const std::string profileName = core::tools::UUID::generateUUID();
    auto profile                  = core::runtime::profile::getCurrentProfile();
    profile->setName(profileName);

    const std::filesystem::path appPrefDir = core::tools::os::getUserDataDir("sight", profileName);
    m_preferencesPath = appPrefDir / "preferences.json";

    // Check preference file dir
    const std::filesystem::path file = gui::preferences::getPreferencesFile();
    CPPUNIT_ASSERT_EQUAL(m_preferencesPath.string(), file.string());
}

//------------------------------------------------------------------------------

void PreferencesTest::helperTest()
{
    const std::string preferenceKey   = "PREF_KEY_TEST";
    const std::string preferenceValue = "PREF_VALUE_TEST";

    core::runtime::profile::Profile::sptr profile = core::runtime::profile::getCurrentProfile();

    const std::filesystem::path appPrefDir = core::tools::os::getUserDataDir("sight", profile->getName());
    const std::filesystem::path prefFile   = appPrefDir / "preferences.json";

    //Check preference file dir
    const std::filesystem::path file = gui::preferences::getPreferencesFile();
    CPPUNIT_ASSERT_EQUAL(prefFile.string(), file.string());

    //Check set preference
    const bool isModified = gui::preferences::setPreference(preferenceKey, preferenceValue);
    CPPUNIT_ASSERT(isModified);

    // Check get preference
    const std::string value = gui::preferences::getPreference(preferenceKey);
    CPPUNIT_ASSERT_EQUAL(preferenceValue, value);

    data::Composite::sptr prefs = gui::preferences::getPreferences();
    CPPUNIT_ASSERT(prefs);

    data::String::sptr prefStr = prefs->at< data::String >(preferenceKey);
    CPPUNIT_ASSERT_EQUAL(preferenceValue, prefStr->value());

    //Check get value
    const std::string preferenceKey2        = "PREF_KEY_TEST_2";
    const std::uint32_t preferenceValueInt2 = 1664;
    const std::string preferenceValue2      = std::to_string(preferenceValueInt2);

    gui::preferences::setPreference(preferenceKey2, preferenceValue2);

    std::string resValue = gui::preferences::getValue(preferenceKey2);
    CPPUNIT_ASSERT_EQUAL(preferenceKey2, resValue);

    std::uint32_t resValueInt = gui::preferences::getValue< std::uint32_t >(preferenceValue2);
    CPPUNIT_ASSERT_EQUAL(preferenceValueInt2, resValueInt);

    const char delimiter                = '%';
    const std::string prefKeySubstitute = delimiter + preferenceKey2 + delimiter;

    resValue = gui::preferences::getValue(prefKeySubstitute);
    CPPUNIT_ASSERT_EQUAL(preferenceValue2, resValue);

    resValueInt = gui::preferences::getValue< std::uint32_t >(prefKeySubstitute);
    CPPUNIT_ASSERT_EQUAL(preferenceValueInt2, resValueInt);
}

//------------------------------------------------------------------------------

void PreferencesTest::passwordTest()
{
    // Reset password field in settings
    gui::preferences::setPassword(std::string());

    // Test default empty password (means no password)
    CPPUNIT_ASSERT_EQUAL(gui::preferences::getPassword(), std::string());

    // Test if there is no hash in preferences (means no password)
    CPPUNIT_ASSERT_EQUAL(gui::preferences::hasPasswordHash(), false);

    // Test with a real password
    const std::string password = "You are the one for me, for me, for me, formidable";
    gui::preferences::setPassword(password);
    CPPUNIT_ASSERT_EQUAL(password, gui::preferences::getPassword());

    // Save the hash
    gui::preferences::savePreferences();

    // Test if there is a hash in preferences
    CPPUNIT_ASSERT_EQUAL(gui::preferences::hasPasswordHash(), true);

    // Verify the good password
    CPPUNIT_ASSERT_EQUAL(gui::preferences::checkPassword(password), true);

    // Verify that bad password doesn't work
    CPPUNIT_ASSERT_EQUAL(gui::preferences::checkPassword("ON DIT CHIFFRER, ET PAS CRYPTER. :-)"), false);
}

//------------------------------------------------------------------------------

void PreferencesTest::cleanup()
{
    // Cleanup
    std::filesystem::remove(m_preferencesPath);
}

//------------------------------------------------------------------------------

} //namespace ut
} //namespace sight::gui::preferences
