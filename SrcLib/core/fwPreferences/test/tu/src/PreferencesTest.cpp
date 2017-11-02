/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2016-2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "PreferencesTest.hpp"

#include <fwPreferences/helper.hpp>

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

    const ::boost::filesystem::path appPrefDir = ::fwTools::os::getUserDataDir("fw4spl", profileName);
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
}

//------------------------------------------------------------------------------

} //namespace ut
} //namespace fwPreferences
