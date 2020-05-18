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

#ifndef __VALIDATORS_UT_MONOCAMERATEST_HPP__
#define __VALIDATORS_UT_MONOCAMERATEST_HPP__

#include <cppunit/extensions/HelperMacros.h>

namespace validators
{
namespace ut
{

/// Test the MonoCamera validator
class MonoCameraTest : public CPPUNIT_NS::TestFixture
{
CPPUNIT_TEST_SUITE( MonoCameraTest );
CPPUNIT_TEST( testValidator );
CPPUNIT_TEST_SUITE_END();

public:
    // interface
    void setUp();
    void tearDown();

    void testValidator();

};

} //namespace ut
} //namespace validators

#endif //__VALIDATORS_UT_MONOCAMERATEST_HPP__

