/************************************************************************
 *
 * Copyright (C) 2009-2022 IRCAD France
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

namespace sight::io::igtl::detail::ut
{

class DataConverterTest : public CPPUNIT_NS::TestFixture
{
CPPUNIT_TEST_SUITE(DataConverterTest);
CPPUNIT_TEST(meshConverterTest);
CPPUNIT_TEST(imageConverterTest);
CPPUNIT_TEST(matrixConverterTest);
CPPUNIT_TEST(pointListConverterTest);
CPPUNIT_TEST(stringConverterTest);
CPPUNIT_TEST(lineConverterTest);
CPPUNIT_TEST(scalarConverterTest);
CPPUNIT_TEST(compositeConverterTest);
CPPUNIT_TEST_SUITE_END();

public:

    void setUp() override;
    void tearDown() override;

    static void imageConverterTest();
    static void matrixConverterTest();
    static void pointListConverterTest();
    static void stringConverterTest();
    static void lineConverterTest();
    static void scalarConverterTest();
    static void meshConverterTest();
    static void compositeConverterTest();
};

} // namespace sight::io::igtl::detail::ut
