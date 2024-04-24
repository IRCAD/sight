/************************************************************************
 *
 * Copyright (C) 2009-2024 IRCAD France
 * Copyright (C) 2012-2020 IHU Strasbourg
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

#include <data/array.hpp>
#include <data/image.hpp>

#include <cppunit/extensions/HelperMacros.h>

namespace sight::io::vtk::ut
{

/**
 * @brief Test many methods to create image.
 */
class image_test : public CPPUNIT_NS::TestFixture
{
CPPUNIT_TEST_SUITE(image_test);
CPPUNIT_TEST(test_image_to_vtk);
CPPUNIT_TEST(test_from_vtk);

CPPUNIT_TEST(from_to_vtk_test);
CPPUNIT_TEST(mhd_reader_test);
CPPUNIT_TEST(mhd_writer_test);
CPPUNIT_TEST(vti_reader_test);

// FIXME: This test is disabled on linux until vtk is patched upstream.
// see https://bugs.debian.org/cgi-bin/bugreport.cgi?bug=1064762
#ifdef WIN32
CPPUNIT_TEST(vti_writer_test);
#endif

CPPUNIT_TEST(vtk_reader_test);
CPPUNIT_TEST(vtk_writer_test);

CPPUNIT_TEST_SUITE_END();

public:

    // interface
    void setUp() override;
    void tearDown() override;

    static void test_image_to_vtk();
    static void test_from_vtk();

    static void from_to_vtk_test();
    static void mhd_reader_test();
    static void mhd_writer_test();
    static void vti_reader_test();
    static void vti_writer_test();
    static void vtk_reader_test();
    static void vtk_writer_test();
};

} // namespace sight::io::vtk::ut
