/************************************************************************
 *
 * Copyright (C) 2023 IRCAD France
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

// cspell:ignore nvjpeg

namespace sight::io::bitmap::ut
{

class ReaderWriterTest : public CPPUNIT_NS::TestFixture
{
public:

    CPPUNIT_TEST_SUITE(ReaderWriterTest);
    CPPUNIT_TEST(grayscaleUINT8Test);
    CPPUNIT_TEST(grayscaleUINT16Test);
    CPPUNIT_TEST(grayscaleINT8Test);
    CPPUNIT_TEST(grayscaleINT16Test);
    CPPUNIT_TEST(rgbUINT8Test);
    CPPUNIT_TEST(rgbUINT16Test);
    CPPUNIT_TEST(rgbINT8Test);
    CPPUNIT_TEST(rgbINT16Test);
    CPPUNIT_TEST(rgbaUINT8Test);
    CPPUNIT_TEST(rgbaUINT16Test);
    CPPUNIT_TEST(rgbaINT8Test);
    CPPUNIT_TEST(rgbaINT16Test);
    CPPUNIT_TEST_SUITE_END();

public:

    void setUp() override;
    void tearDown() override;

    static void grayscaleUINT8Test();
    static void grayscaleUINT16Test();
    static void grayscaleINT8Test();
    static void grayscaleINT16Test();
    static void rgbUINT8Test();
    static void rgbUINT16Test();
    static void rgbINT8Test();
    static void rgbINT16Test();
    static void rgbaUINT8Test();
    static void rgbaUINT16Test();
    static void rgbaINT8Test();
    static void rgbaINT16Test();
};

} // namespace sight::io::bitmap::ut
