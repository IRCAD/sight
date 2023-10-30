/************************************************************************
 *
 * Copyright (C) 2016-2023 IRCAD France
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

#pragma once

#include <activity/extension/activity.hpp>

#include <core/macros.hpp>
#include <core/runtime/module.hpp>

#include <cppunit/extensions/HelperMacros.h>

namespace sight::activity::ut
{

class default_activity_test : public CPPUNIT_NS::TestFixture
{
CPPUNIT_TEST_SUITE(default_activity_test);

CPPUNIT_TEST(requirements_test);
CPPUNIT_TEST(object_test);
CPPUNIT_TEST(validator_test);
CPPUNIT_TEST(big_activity_test);

CPPUNIT_TEST_SUITE_END();

public:

    // interface
    void setUp() override;
    void tearDown() override;

    static void requirements_test();
    static void object_test();
    static void validator_test();
    static void big_activity_test();
};

} // namespace sight::activity::ut
