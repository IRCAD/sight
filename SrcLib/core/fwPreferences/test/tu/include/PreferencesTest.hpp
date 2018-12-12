/************************************************************************
 *
 * Copyright (C) 2016 IRCAD France
 * Copyright (C) 2016 IHU Strasbourg
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
