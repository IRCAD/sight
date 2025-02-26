/************************************************************************
 *
 * Copyright (C) 2009-2024 IRCAD France
 * Copyright (C) 2012-2015 IHU Strasbourg
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

#include <data/image.hpp>
#include <data/matrix4.hpp>

#include <service/macros.hpp>

#include <cppunit/extensions/HelperMacros.h>

namespace sight::io::itk::ut
{

class image_reader_writer_test : public CPPUNIT_NS::TestFixture
{
CPPUNIT_TEST_SUITE(image_reader_writer_test);
CPPUNIT_TEST(inr_read_write_test);
CPPUNIT_TEST(inr_stress_test);
CPPUNIT_TEST(nifti_read_test);
CPPUNIT_TEST(nifti_write_test);
CPPUNIT_TEST(jpeg_write_test);
CPPUNIT_TEST(inr_read_jpeg_write_test);
CPPUNIT_TEST_SUITE_END();

public:

    // interface
    void setUp() override;
    void tearDown() override;

    static void inr_read_write_test();
    static void inr_stress_test();
    static void nifti_read_test();
    static void nifti_write_test();
    static void jpeg_write_test();
    static void inr_read_jpeg_write_test();

private:

    static void inr_stress_test_with_type(core::type _type, int _nb_test);
    static void inr_read_write_check(data::image::sptr _image);
    static void nifti_read_check(
        const data::image::sptr& _image_to_test,
        const core::type& _expected_type,
        std::size_t _expected_dim,
        const data::image::spacing_t& _expected_spacing,
        const data::image::origin_t& _expected_origin,
        const data::image::size_t& _expected_size,
        const data::image::orientation_t& _expected_orientation
    );
};

} // namespace sight::io::itk::ut
