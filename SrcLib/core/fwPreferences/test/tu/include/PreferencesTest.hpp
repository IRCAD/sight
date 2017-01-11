/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __FWPREFERENCES_UT_PREFERENCESTEST_HPP__
#define __FWPREFERENCES_UT_PREFERENCESTEST_HPP__

#include <fwRuntime/EConfigurationElement.hpp>
#include <fwRuntime/profile/Profile.hpp>

#include <cppunit/extensions/HelperMacros.h>

namespace fwPreferences
{
namespace ut
{

/**
 * @brief Preferences unit test .
 */
class PreferencesTest : public CPPUNIT_NS::TestFixture
{
CPPUNIT_TEST_SUITE( PreferencesTest );
CPPUNIT_TEST( helperTest );
CPPUNIT_TEST_SUITE_END();


public:
    // interface
    void setUp();
    void tearDown();

    void helperTest();

private:
    ::fwRuntime::profile::Profile::sptr m_profile;
};

} //namespace ut
} //namespace fwPreferences

#endif // __FWPREFERENCES_UT_PREFERENCESTEST_HPP__
