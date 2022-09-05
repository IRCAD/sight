/************************************************************************
 *
 * Copyright (C) 2022 IRCAD France
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

namespace sight::core::tools::ut
{

class CompareTest : public CPPUNIT_NS::TestFixture
{
CPPUNIT_TEST_SUITE(CompareTest);
CPPUNIT_TEST(doubleContainerTest);
CPPUNIT_TEST(floatContainerTest);
CPPUNIT_TEST(mixedContainerTest);
CPPUNIT_TEST(sandboxTest);
CPPUNIT_TEST(infiniteTest);
CPPUNIT_TEST(nanTest);
CPPUNIT_TEST(pointerTest);
CPPUNIT_TEST(mapTest);
CPPUNIT_TEST(unorderedSetTest);
CPPUNIT_TEST(pairTest);
CPPUNIT_TEST_SUITE_END();

public:

    // interface
    void setUp() override;
    void tearDown() override;

    static void doubleContainerTest();
    static void floatContainerTest();
    static void mixedContainerTest();

    static void sandboxTest();
    static void infiniteTest();
    static void nanTest();

    static void pointerTest();
    static void mapTest();
    static void unorderedSetTest();
    static void pairTest();
};

} // namespace sight::core::tools::ut
