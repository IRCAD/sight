/************************************************************************
 *
 * Copyright (C) 2009-2023 IRCAD France
 * Copyright (C) 2012-2016 IHU Strasbourg
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

class GenericTLTest : public CPPUNIT_NS::TestFixture
{
public:

    CPPUNIT_TEST_SUITE(GenericTLTest);
    CPPUNIT_TEST(pushPopTest);
    CPPUNIT_TEST(pushClassTest);
    CPPUNIT_TEST(copyTest);
    CPPUNIT_TEST(iteratorTest);
    CPPUNIT_TEST(objectValid);
    CPPUNIT_TEST(equalityTest);
    CPPUNIT_TEST_SUITE_END();

public:

    // interface
    void setUp() override;
    void tearDown() override;

    static void pushPopTest();
    static void pushClassTest();
    static void copyTest();
    static void iteratorTest();
    static void objectValid();
    static void equalityTest();
};

} // namespace sight::data::ut
