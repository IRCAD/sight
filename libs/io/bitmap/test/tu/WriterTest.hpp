/************************************************************************
 *
 * Copyright (C) 2021-2023 IRCAD France
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

// cspell:ignore nvjpeg

namespace sight::io::bitmap::ut
{

class writer_test : public CPPUNIT_NS::TestFixture
{
public:

    CPPUNIT_TEST_SUITE(writer_test);
    CPPUNIT_TEST(basic_test);
    CPPUNIT_TEST(extensions_test);
    CPPUNIT_TEST(wildcard_test);
    CPPUNIT_TEST(conformance_test);
    CPPUNIT_TEST(empty_image_test);
    CPPUNIT_TEST(wrong_path_test);
    CPPUNIT_TEST(from_dicom_test);
    CPPUNIT_TEST(profiling_test);
    CPPUNIT_TEST_SUITE_END();

public:

    void setUp() override;
    void tearDown() override;

    static void basic_test();
    static void extensions_test();
    static void wildcard_test();
    static void conformance_test();
    static void empty_image_test();
    static void wrong_path_test();
    static void from_dicom_test();
    static void profiling_test();
};

} // namespace sight::io::bitmap::ut
