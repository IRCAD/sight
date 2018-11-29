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

#ifndef __FWACTIVITIES_UT_VALIDATOR_DEFAULTACTIVITYTEST_HPP__
#define __FWACTIVITIES_UT_VALIDATOR_DEFAULTACTIVITYTEST_HPP__

#include <cppunit/extensions/HelperMacros.h>

#include <fwActivities/registry/Activities.hpp>

#include <fwCore/macros.hpp>

#include <fwRuntime/Bundle.hpp>

namespace fwActivities
{

namespace ut
{

class DefaultActivityTest : public CPPUNIT_NS::TestFixture
{
CPPUNIT_TEST_SUITE( DefaultActivityTest );

CPPUNIT_TEST(requirementsTest);
CPPUNIT_TEST(parametersTest);
CPPUNIT_TEST(objectTest);
CPPUNIT_TEST(validatorTest);
CPPUNIT_TEST(bigActivityTest);

CPPUNIT_TEST_SUITE_END();

public:
    // interface
    void setUp();
    void tearDown();

    void parametersTest();
    void requirementsTest();
    void objectTest();
    void validatorTest();
    void bigActivityTest();

protected:
    SPTR( ::fwRuntime::Bundle ) m_bundle;
};

} //namespace ut
} //namespace fwActivities

#endif //__FWACTIVITIES_UT_VALIDATOR_DEFAULTACTIVITYTEST_HPP__

