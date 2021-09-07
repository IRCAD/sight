/************************************************************************
 *
 * Copyright (C) 2009-2021 IRCAD France
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

namespace sight::data
{

namespace ut
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
    CPPUNIT_TEST_SUITE_END();

public:

    // interface
    void setUp();
    void tearDown();

    void allocation();
    void resize();
    void reallocate();
    void copy();
    void dumpLockTest();
    void bufferAccessTest();
    void constArrayTest();
    void emptyIteratorTest();
};

} //namespace ut

} //namespace sight::data
