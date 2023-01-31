/************************************************************************
 *
 * Copyright (C) 2009-2022 IRCAD France
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

class ArrayTest : public CPPUNIT_NS::TestFixture
{
private:

    CPPUNIT_TEST_SUITE(ArrayTest);
    CPPUNIT_TEST(allocation);
    CPPUNIT_TEST(resize);
    CPPUNIT_TEST(reallocate);
    CPPUNIT_TEST(copy);
    CPPUNIT_TEST(dumpLockTest);
    CPPUNIT_TEST(bufferAccessTest);
    CPPUNIT_TEST(constArrayTest);
    CPPUNIT_TEST(emptyIteratorTest);
    CPPUNIT_TEST(equalityTest);
    CPPUNIT_TEST(swapTest);
    CPPUNIT_TEST(resizeNonOwnerTest);
    CPPUNIT_TEST(setBufferObjectNullThenResizeTest);
    CPPUNIT_TEST_SUITE_END();

public:

    // interface
    void setUp() override;
    void tearDown() override;

    static void allocation();
    static void resize();
    static void reallocate();
    static void copy();
    static void dumpLockTest();
    static void bufferAccessTest();
    static void constArrayTest();
    static void emptyIteratorTest();
    static void equalityTest();
    static void swapTest();
    static void resizeNonOwnerTest();
    static void setBufferObjectNullThenResizeTest();
    static void atTest();
};

} // namespace sight::data::ut
