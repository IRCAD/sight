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

class ReaderTest : public CPPUNIT_NS::TestFixture
{
public:

    CPPUNIT_TEST_SUITE(ReaderTest);
    CPPUNIT_TEST(basicTest);
    CPPUNIT_TEST(extensionsTest);
    CPPUNIT_TEST(wildcardTest);
    CPPUNIT_TEST(nvJPEGTest);
    CPPUNIT_TEST(nvJPEG2KTest);
    CPPUNIT_TEST(libPNGTest);
    CPPUNIT_TEST(libJPEGTest);
    CPPUNIT_TEST(openJPEGTest);
    CPPUNIT_TEST(libTIFFTest);
    CPPUNIT_TEST(profilingTest);
    CPPUNIT_TEST_SUITE_END();

public:

    void setUp() override;
    void tearDown() override;

    static void basicTest();
    static void extensionsTest();
    static void wildcardTest();

    static void nvJPEGTest();
    static void nvJPEG2KTest();
    static void libPNGTest();
    static void libJPEGTest();
    static void openJPEGTest();
    static void libTIFFTest();

    static void profilingTest();
};

} // namespace sight::io::bitmap::ut
