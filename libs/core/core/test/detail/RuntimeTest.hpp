/************************************************************************
 *
 * Copyright (C) 2009-2021 IRCAD France
 * Copyright (C) 2012-2020 IHU Strasbourg
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

#include <cppunit/extensions/HelperMacros.h>

namespace sight::core::runtime
{

namespace detail
{

namespace ut
{

/**
 * @brief   Test Runtime : read the modules in CommonLib
 */
class RuntimeTest : public CPPUNIT_NS::TestFixture
{
CPPUNIT_TEST_SUITE(RuntimeTest);

#if defined(linux) || defined(__linux) || defined(__APPLE__)
CPPUNIT_TEST(testPosix);
#elif defined(WIN32)
CPPUNIT_TEST(testWin32);
#endif

CPPUNIT_TEST(testRuntime);
CPPUNIT_TEST_SUITE_END();

public:

    RuntimeTest();

    // interface
    void setUp();
    void tearDown();

#if defined(linux) || defined(__linux) || defined(__APPLE__)
    void testPosix();
#endif
#if defined(WIN32)
    void testWin32();
#endif
    void testRuntime();
};

} // namespace ut

} // namespace detail

} // namespace fwRuntime
