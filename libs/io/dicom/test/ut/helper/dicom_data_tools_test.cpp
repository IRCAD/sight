/************************************************************************
 *
 * Copyright (C) 2022-2024 IRCAD France
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

#include "dicom_data_tools_test.hpp"

#include <core/type.hpp>

#include <io/dicom/helper/dicom_data_tools.hpp>

#include <gdcmPixelFormat.h>

CPPUNIT_TEST_SUITE_REGISTRATION(sight::io::dicom::helper::ut::dicom_data_tools_test);

namespace sight::io::dicom::helper::ut
{

using helper = helper::dicom_data_tools;

//------------------------------------------------------------------------------

void dicom_data_tools_test::get_pixel_type_test()
{
    CPPUNIT_ASSERT_EQUAL(gdcm::PixelFormat(gdcm::PixelFormat::UINT8), helper::get_pixel_type(core::type::UINT8));
    CPPUNIT_ASSERT_EQUAL(gdcm::PixelFormat(gdcm::PixelFormat::INT8), helper::get_pixel_type(core::type::INT8));
    CPPUNIT_ASSERT_EQUAL(gdcm::PixelFormat(gdcm::PixelFormat::UINT16), helper::get_pixel_type(core::type::UINT16));
    CPPUNIT_ASSERT_EQUAL(gdcm::PixelFormat(gdcm::PixelFormat::INT16), helper::get_pixel_type(core::type::INT16));
    CPPUNIT_ASSERT_EQUAL(gdcm::PixelFormat(gdcm::PixelFormat::UINT32), helper::get_pixel_type(core::type::UINT32));
    CPPUNIT_ASSERT_EQUAL(gdcm::PixelFormat(gdcm::PixelFormat::INT32), helper::get_pixel_type(core::type::INT32));
    CPPUNIT_ASSERT_EQUAL(gdcm::PixelFormat(gdcm::PixelFormat::FLOAT32), helper::get_pixel_type(core::type::FLOAT));
    CPPUNIT_ASSERT_EQUAL(gdcm::PixelFormat(gdcm::PixelFormat::FLOAT64), helper::get_pixel_type(core::type::DOUBLE));
    CPPUNIT_ASSERT_EQUAL(gdcm::PixelFormat(gdcm::PixelFormat::UNKNOWN), helper::get_pixel_type(core::type::NONE));
}

//------------------------------------------------------------------------------

void dicom_data_tools_test::get_photometric_interpretation_test()
{
    using gdcm_pi = gdcm::PhotometricInterpretation;

    auto image = std::make_shared<data::image>();

    image->resize({1}, core::type::UINT8, data::image::pixel_format_t::gray_scale);
    CPPUNIT_ASSERT_EQUAL(gdcm_pi(gdcm_pi::MONOCHROME2), helper::get_photometric_interpretation(image));

    image->resize({1}, core::type::UINT8, data::image::pixel_format_t::rg);
    CPPUNIT_ASSERT_EQUAL(gdcm_pi(gdcm_pi::UNKNOWN), helper::get_photometric_interpretation(image));

    image->resize({1}, core::type::UINT8, data::image::pixel_format_t::rgb);
    CPPUNIT_ASSERT_EQUAL(gdcm_pi(gdcm_pi::RGB), helper::get_photometric_interpretation(image));

    image->resize({1}, core::type::UINT8, data::image::pixel_format_t::rgba);
    CPPUNIT_ASSERT_EQUAL(gdcm_pi(gdcm_pi::ARGB), helper::get_photometric_interpretation(image));
}

//------------------------------------------------------------------------------

void dicom_data_tools_test::convert_to_presentation_type_test()
{
    CPPUNIT_ASSERT_EQUAL(gdcm::Surface::SURFACE, helper::convert_to_presentation_type(data::material::surface));
    CPPUNIT_ASSERT_EQUAL(gdcm::Surface::POINTS, helper::convert_to_presentation_type(data::material::point));
    CPPUNIT_ASSERT_EQUAL(gdcm::Surface::WIREFRAME, helper::convert_to_presentation_type(data::material::wireframe));
    CPPUNIT_ASSERT_EQUAL(gdcm::Surface::SURFACE, helper::convert_to_presentation_type(data::material::edge));
}

//------------------------------------------------------------------------------

void dicom_data_tools_test::convert_to_representation_mode_test()
{
    CPPUNIT_ASSERT_EQUAL(data::material::surface, helper::convert_to_representation_mode(gdcm::Surface::SURFACE));
    CPPUNIT_ASSERT_EQUAL(data::material::wireframe, helper::convert_to_representation_mode(gdcm::Surface::WIREFRAME));
    CPPUNIT_ASSERT_EQUAL(data::material::point, helper::convert_to_representation_mode(gdcm::Surface::POINTS));
}

//------------------------------------------------------------------------------

void dicom_data_tools_test::convert_point_to_frame_number_nominal_test()
{
    auto image = std::make_shared<data::image>();
    image->resize({1, 2, 3}, core::type::UINT8, data::image::pixel_format_t::gray_scale);
    image->set_spacing({1, 1, 1});
    CPPUNIT_ASSERT_EQUAL(
        std::size_t(3),
        helper::convert_point_to_frame_number(image, std::make_shared<data::point>(0., 1., 2.))
    );
}

//------------------------------------------------------------------------------

void dicom_data_tools_test::convert_point_to_frame_number_failure_test()
{
    auto image = std::make_shared<data::image>();
    image->resize({1, 2, 3}, core::type::UINT8, data::image::pixel_format_t::gray_scale);
    image->set_spacing({1, 1, 1});
    CPPUNIT_ASSERT_THROW(
        helper::convert_point_to_frame_number(image, std::make_shared<data::point>(1., 2., 3.)),
        io::dicom::exception::failed
    );
}

//------------------------------------------------------------------------------

void dicom_data_tools_test::convert_frame_number_to_z_coordinate_nominal_test()
{
    auto image = std::make_shared<data::image>();
    image->resize({1, 2, 3}, core::type::UINT8, data::image::pixel_format_t::gray_scale);
    image->set_spacing({1, 1, 1});
    CPPUNIT_ASSERT_EQUAL(2., helper::convert_frame_number_to_z_coordinate(image, 3));
}

//------------------------------------------------------------------------------

void dicom_data_tools_test::convert_frame_number_to_z_coordinate_failure_test()
{
    auto image = std::make_shared<data::image>();
    image->resize({1, 2, 3}, core::type::UINT8, data::image::pixel_format_t::gray_scale);
    image->set_spacing({1, 1, 1});
    CPPUNIT_ASSERT_THROW(helper::convert_frame_number_to_z_coordinate(image, 0), io::dicom::exception::failed);
}

} // namespace sight::io::dicom::helper::ut
