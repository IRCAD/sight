/************************************************************************
 *
 * Copyright (C) 2009-2024 IRCAD France
 * Copyright (C) 2012-2021 IHU Strasbourg
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

namespace sight::data::ut
{

class integer_test : public CPPUNIT_NS::TestFixture
{
private:

    CPPUNIT_TEST_SUITE(integer_test);
    CPPUNIT_TEST(basic);
    CPPUNIT_TEST(string_conversion);
    CPPUNIT_TEST(reset);
    CPPUNIT_TEST_SUITE_END();

protected:

public:

    // interface
    void setUp() override;
    void tearDown() override;

    static void basic();
    static void string_conversion();
    static void reset();
};

} // namespace sight::data::ut
