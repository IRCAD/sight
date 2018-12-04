/************************************************************************
 *
 * Copyright (C) 2016-2018 IRCAD France
 * Copyright (C) 2016-2018 IHU Strasbourg
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

#include <fwPreferences/helper.hpp>

#include <fwData/Composite.hpp>
#include <fwData/String.hpp>

#include <fwRuntime/Bundle.hpp>
#include <fwRuntime/IPlugin.hpp>
#include <fwRuntime/operations.hpp>
#include <fwRuntime/Runtime.hpp>

#include <fwTools/Os.hpp>

#include <boost/filesystem/path.hpp>

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION( ::fwPreferences::ut::PreferencesTest );

namespace fwPreferences
{
namespace ut
{

//------------------------------------------------------------------------------

void PreferencesTest::setUp()
{
    m_profile = ::fwRuntime::profile::Profile::New();
    m_profile->setName("APP_TEST");
    ::fwRuntime::profile::setCurrentProfile(m_profile);

    ::fwRuntime::Runtime* runtime = ::fwRuntime::Runtime::getDefault();
    runtime->addDefaultBundles();

    std::shared_ptr< ::fwRuntime::Bundle > dataReg = runtime->findBundle("dataReg");
    CPPUNIT_ASSERT_MESSAGE("'dataReg bundle not found !'", dataReg);
    dataReg->setEnable(true);
    CPPUNIT_ASSERT(dataReg->isEnable());
    dataReg->start();

    std::shared_ptr< ::fwRuntime::Bundle > servicesReg = runtime->findBundle("servicesReg");
    CPPUNIT_ASSERT_MESSAGE("'servicesReg bundle not found !'", servicesReg);
    servicesReg->setEnable(true);
    CPPUNIT_ASSERT(servicesReg->isEnable());
    servicesReg->start();

    std::shared_ptr< ::fwRuntime::Bundle > preferences = runtime->findBundle("preferences");
    CPPUNIT_ASSERT_MESSAGE("'preferences bundle not found !'", preferences);
    preferences->setEnable(true);
    CPPUNIT_ASSERT(preferences->isEnable());
    preferences->start();
}

//------------------------------------------------------------------------------

void PreferencesTest::tearDown()
{
    // Clean up after the test run.
}

//------------------------------------------------------------------------------

void PreferencesTest::helperTest()
{
    const std::string preferenceKey   = "PREF_KEY_TEST";
    const std::string preferenceValue = "PREF_VALUE_TEST";
    const std::string profileName     = m_profile->getName();

    const ::boost::filesystem::path appPrefDir = ::fwTools::os::getUserDataDir("sight", profileName);
    const ::boost::filesystem::path prefFile   = appPrefDir / "preferences.json";

    //Check preference file dir
    const ::boost::filesystem::path file = ::fwPreferences::getPreferencesFile();
    CPPUNIT_ASSERT_EQUAL(prefFile.string(), file.string());

    //Check set preference
    const bool isModified = ::fwPreferences::setPreference(preferenceKey, preferenceValue);
    CPPUNIT_ASSERT(isModified);

    //Check get preference
    const std::string value = ::fwPreferences::getPreference(preferenceKey);
    CPPUNIT_ASSERT_EQUAL(preferenceValue, value);

    ::fwData::Composite::sptr prefs = ::fwPreferences::getPreferences();
    CPPUNIT_ASSERT(prefs);

    ::fwData::String::sptr prefStr = prefs->at< ::fwData::String >(preferenceKey);
    CPPUNIT_ASSERT_EQUAL(preferenceValue, prefStr->value());
}

//------------------------------------------------------------------------------

} //namespace ut
} //namespace fwPreferences
