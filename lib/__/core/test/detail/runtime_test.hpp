/************************************************************************
 *
 * Copyright (C) 2009-2023 IRCAD France
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

namespace sight::core::runtime::detail::ut
{

/**
 * @brief   Test Runtime : read the modules in CommonLib
 */
class runtime_test : public CPPUNIT_NS::TestFixture
{
CPPUNIT_TEST_SUITE(runtime_test);

#if defined(__unix__)
CPPUNIT_TEST(test_posix);
#elif defined(WIN32)
CPPUNIT_TEST(test_win32);
#endif

CPPUNIT_TEST(test_runtime);
CPPUNIT_TEST_SUITE_END();

public:

    runtime_test();

    // interface
    void setUp() override;
    void tearDown() override;

#if defined(__unix__)
    static void test_posix();
#endif
#if defined(WIN32)
    void test_win32();
#endif
    static void test_runtime();
};

} // namespace sight::core::runtime::detail::ut
