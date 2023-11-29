/************************************************************************
 *
 * Copyright (C) 2022-2023 IRCAD France
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

namespace sight::data::tools::ut
{

class container_notifier_test : public CPPUNIT_NS::TestFixture
{
CPPUNIT_TEST_SUITE(container_notifier_test);
CPPUNIT_TEST(vector_test);
CPPUNIT_TEST(composite_test);
CPPUNIT_TEST(series_set_test);
CPPUNIT_TEST(activity_set_test);
CPPUNIT_TEST(camera_set_test);
CPPUNIT_TEST_SUITE_END();

public:

    void setUp() override;
    void tearDown() override;

    static void vector_test();
    static void composite_test();
    static void series_set_test();
    static void activity_set_test();
    static void camera_set_test();
};

} // namespace sight::data::tools::ut
