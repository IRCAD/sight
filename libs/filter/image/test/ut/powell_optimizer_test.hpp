/************************************************************************
 *
 * Copyright (C) 2018-2023 IRCAD France
 * Copyright (C) 2018-2021 IHU Strasbourg
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

namespace sight::filter::image::ut
{

/**
 * @brief Optimizer tests. Find the minimum of some popular functions.
 *
 * Common test functions were found [here](https://en.wikipedia.org/wiki/Test_functions_for_optimization).
 */
class powell_optimizer_test : public CPPUNIT_NS::TestFixture
{
CPPUNIT_TEST_SUITE(powell_optimizer_test);
CPPUNIT_TEST(parabola_test);
CPPUNIT_TEST(ackley_test);
CPPUNIT_TEST(sphere_function_test);
CPPUNIT_TEST(beale_function_test);
CPPUNIT_TEST_SUITE_END();

public:

    // interface
    void setUp() override;
    void tearDown() override;

    static void parabola_test();
    static void ackley_test();
    static void sphere_function_test();
    static void beale_function_test();
};

} // namespace sight::filter::image::ut
