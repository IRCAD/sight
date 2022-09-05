/************************************************************************
 *
 * Copyright (C) 2021-2022 IRCAD France
 * Copyright (C) 2017 IHU Strasbourg
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

namespace sight::io::opencv::ut
{

/**
 * @brief Tests the io::opencv::Image functions
 */
class TypeTest : public CPPUNIT_NS::TestFixture
{
public:

    CPPUNIT_TEST_SUITE(TypeTest);
    CPPUNIT_TEST(toCv);
    CPPUNIT_TEST(fromCv);
    CPPUNIT_TEST_SUITE_END();

public:

    // interface
    void setUp() override;
    void tearDown() override;

    static void toCv();
    static void fromCv();
};

} // namespace sight::io::opencv::ut
