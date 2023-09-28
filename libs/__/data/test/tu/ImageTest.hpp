/************************************************************************
 *
 * Copyright (C) 2009-2023 IRCAD France
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

class ImageTest : public CPPUNIT_NS::TestFixture
{
private:

    CPPUNIT_TEST_SUITE(ImageTest);
    CPPUNIT_TEST(testGetterSetter);
    CPPUNIT_TEST(testAllocation);
    CPPUNIT_TEST(testReallocation);
    CPPUNIT_TEST(testSetGetPixel);
    CPPUNIT_TEST(testSetGetPixelRGBA);
    CPPUNIT_TEST(testIterator);
    CPPUNIT_TEST(testRGBIterator);
    CPPUNIT_TEST(testBGRIterator);
    CPPUNIT_TEST(testRGBAIterator);
    CPPUNIT_TEST(testBGRAIterator);
    CPPUNIT_TEST(benchmarkIterator);
    CPPUNIT_TEST(imageDeepCopy);
    CPPUNIT_TEST(emptyIteratorTest);
    CPPUNIT_TEST(equalityTest);
    CPPUNIT_TEST_SUITE_END();

public:

    // interface
    void setUp() override;
    void tearDown() override;

    static void testGetterSetter();
    static void testAllocation();
    static void testReallocation();
    static void testSetGetPixel();
    static void testSetGetPixelRGBA();
    static void testIterator();
    static void testRGBIterator();
    static void testBGRIterator();
    static void testRGBAIterator();
    static void testBGRAIterator();
    static void benchmarkIterator();
    static void imageDeepCopy();
    static void emptyIteratorTest();
    static void equalityTest();
};

} // namespace sight::data::ut
