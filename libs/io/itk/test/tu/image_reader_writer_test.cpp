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

#include "image_reader_writer_test.hpp"

#include "helper.hpp"

#include <core/base.hpp>
#include <core/os/temp_path.hpp>

#include <io/itk/inr_image_reader.hpp>
#include <io/itk/inr_image_writer.hpp>
#include <io/itk/jpg_image_writer.hpp>
#include <io/itk/nifti_image_reader.hpp>
#include <io/itk/nifti_image_writer.hpp>

#include <utest_data/data.hpp>
#include <utest_data/generator/image.hpp>

#include <filesystem>

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION(sight::io::itk::ut::image_reader_writer_test);

namespace sight::io::itk::ut
{

static const double EPSILON = 0.00001;

//------------------------------------------------------------------------------

void image_reader_writer_test::setUp()
{
}

//------------------------------------------------------------------------------

void image_reader_writer_test::tearDown()
{
}

//------------------------------------------------------------------------------

void image_reader_writer_test::inr_read_write_test()
{
    // create image
    data::image::sptr image = std::make_shared<data::image>();
    utest_data::generator::image::generate_random_image(image, core::type::INT16);
    sight::io::itk::ut::image_reader_writer_test::inr_read_write_check(image);
}

//------------------------------------------------------------------------------

void image_reader_writer_test::inr_stress_test()
{
    core::type type = core::type::UINT8;
    sight::io::itk::ut::image_reader_writer_test::inr_stress_test_with_type(type, 5);

    type = core::type::INT16;
    sight::io::itk::ut::image_reader_writer_test::inr_stress_test_with_type(type, 5);

    type = core::type::UINT16;
    sight::io::itk::ut::image_reader_writer_test::inr_stress_test_with_type(type, 5);

    type = core::type::INT32;
    sight::io::itk::ut::image_reader_writer_test::inr_stress_test_with_type(type, 5);

    type = core::type::UINT32;
    sight::io::itk::ut::image_reader_writer_test::inr_stress_test_with_type(type, 5);

    type = core::type::FLOAT;
    sight::io::itk::ut::image_reader_writer_test::inr_stress_test_with_type(type, 5);
}

//------------------------------------------------------------------------------

void image_reader_writer_test::nifti_read_test()
{
    //cspell: ignore 3Dkidney
    const std::filesystem::path sight_image_path(utest_data::dir() / "sight/image/nii/3Dkidney.nii");

    CPPUNIT_ASSERT_MESSAGE(
        "The file '" + sight_image_path.string() + "' does not exist",
        std::filesystem::exists(sight_image_path)
    );

    // load image
    data::image::sptr sight_image                        = std::make_shared<data::image>();
    io::itk::nifti_image_reader::sptr sight_image_reader = std::make_shared<io::itk::nifti_image_reader>();
    sight_image_reader->set_object(sight_image);
    sight_image_reader->set_file(sight_image_path);
    sight_image_reader->read();

    nifti_read_check(
        sight_image,
        core::type::UINT8,
        3,
        {0.744924, 0.744924, 0.744924},
        {0, 0, 0},
        {481, 362, 478
        });

    const std::filesystem::path external_image_path(utest_data::dir() / "sight/image/nii/brain.nii");

    CPPUNIT_ASSERT_MESSAGE(
        "The file '" + external_image_path.string() + "' does not exist",
        std::filesystem::exists(external_image_path)
    );

    // load image
    data::image::sptr external_image                        = std::make_shared<data::image>();
    io::itk::nifti_image_reader::sptr external_image_reader = std::make_shared<io::itk::nifti_image_reader>();
    external_image_reader->set_object(external_image);
    external_image_reader->set_file(external_image_path);
    external_image_reader->read();

    nifti_read_check(
        external_image,
        core::type::UINT8,
        3,
        {0.5, 0.5, 0.5},
        {75, 107, -69.5},
        {301, 370, 316
        });
}

//------------------------------------------------------------------------------

void image_reader_writer_test::nifti_read_check(
    const data::image::sptr& _image_to_test,
    const core::type& _expected_type,
    const std::size_t _expected_dim,
    const data::image::spacing_t& _expected_spacing,
    const data::image::origin_t& _expected_origin,
    const data::image::size_t& _expected_size
)
{
    CPPUNIT_ASSERT_EQUAL_MESSAGE(
        "test on sight/image/nii/3Dkidney.nii failed ",
        _image_to_test->type(),
        _expected_type
    );

    CPPUNIT_ASSERT_EQUAL(
        static_cast<std::size_t>(_image_to_test->num_dimensions()),
        _expected_dim
    );

    for(std::size_t i = 0 ; i < _expected_dim ; ++i)
    {
        CPPUNIT_ASSERT_DOUBLES_EQUAL(
            static_cast<data::image::spacing_t::value_type>(_image_to_test->spacing()[i]),
            static_cast<data::image::spacing_t::value_type>(_expected_spacing[i]),
            EPSILON
        );
        CPPUNIT_ASSERT_DOUBLES_EQUAL(
            static_cast<data::image::origin_t::value_type>(_image_to_test->origin()[i]),
            static_cast<data::image::origin_t::value_type>(_expected_origin[i]),
            EPSILON
        );
        CPPUNIT_ASSERT_EQUAL(
            static_cast<data::image::size_t::value_type>(_image_to_test->size()[i]),
            static_cast<data::image::size_t::value_type>(_expected_size[i])
        );
    }
}

//------------------------------------------------------------------------------

void image_reader_writer_test::nifti_write_test()
{
    // create image
    data::image::sptr image = std::make_shared<data::image>();
    utest_data::generator::image::generate_random_image(image, core::type::INT16);

    // WARNING!
    // There is a conversion subtlety. Nifti stores the data as float, and convert them.
    // The writing/reading generates a conversion double => float => double.
    // This leads to several errors.
    // To prevent this, the spacing and origin used for the test are set as float, converted to double, to be able to
    // test the direct equality.
    const data::image::spacing_t spacing_d = {0.5F, 0.001F, 1.25F};
    const data::image::origin_t origin_d   = {0.5F, 0.25F, 0.25F};
    image->set_spacing(spacing_d);
    image->set_origin(origin_d);

    // save image in nifti
    core::os::temp_dir tmp_dir;
    const std::filesystem::path filename = tmp_dir / "image.nii";
    auto my_writer                       = std::make_shared<io::itk::nifti_image_writer>();

    my_writer->set_object(image);
    my_writer->set_file(filename);
    my_writer->write();

    CPPUNIT_ASSERT_MESSAGE(
        "test on '" + filename.string() + "' failed ",
        std::filesystem::exists(filename)
    );

    // load image
    data::image::sptr image2 = std::make_shared<data::image>();
    image2->set_window_center(image->window_center());
    image2->set_window_width(image->window_width());

    auto my_reader = std::make_shared<io::itk::nifti_image_reader>();
    my_reader->set_object(image2);
    my_reader->set_file(filename);
    my_reader->read();

    CPPUNIT_ASSERT(*image == *image2);
}

//------------------------------------------------------------------------------

void image_reader_writer_test::jpeg_write_test()
{
    // create image
    data::image::sptr image = std::make_shared<data::image>();
    utest_data::generator::image::generate_random_image(image, core::type::INT16);

    // save image in inr
    core::os::temp_dir tmp_dir;
    auto my_writer = std::make_shared<io::itk::jpg_image_writer>();
    my_writer->set_object(image);
    my_writer->set_folder(tmp_dir);
    CPPUNIT_ASSERT_NO_THROW(my_writer->write());
}

//------------------------------------------------------------------------------

void image_reader_writer_test::inr_read_jpeg_write_test()
{
    // create image
    std::filesystem::path path_inr = utest_data::dir() / "sight" / "image" / "inr" / "image.inr.gz";

    CPPUNIT_ASSERT_MESSAGE(
        "The file '" + path_inr.string() + "' does not exist",
        std::filesystem::exists(path_inr)
    );

    data::image::sptr image                   = std::make_shared<data::image>();
    io::itk::inr_image_reader::sptr my_reader = std::make_shared<io::itk::inr_image_reader>();
    my_reader->set_object(image);
    my_reader->set_file(path_inr);
    my_reader->read();

    // save image in inr
    core::os::temp_dir tmp_dir;
    auto my_writer = std::make_shared<io::itk::jpg_image_writer>();
    my_writer->set_object(image);
    my_writer->set_folder(tmp_dir);
    CPPUNIT_ASSERT_NO_THROW(my_writer->write());
}

//------------------------------------------------------------------------------

void image_reader_writer_test::inr_stress_test_with_type(core::type _type, int _nb_test)
{
    for(int nb = 0 ; nb < _nb_test ; ++nb)
    {
        data::image::sptr image = std::make_shared<data::image>();
        utest_data::generator::image::generate_random_image(image, _type);
        sight::io::itk::ut::image_reader_writer_test::inr_read_write_check(image);
    }
}

//------------------------------------------------------------------------------

void image_reader_writer_test::inr_read_write_check(data::image::sptr _image)
{
    // inr only support image origin (0,0,0)
    const data::image::origin_t origin = {0., 0., 0.};
    _image->set_origin(origin);

    // save image in inr
    core::os::temp_dir tmp_dir;
    const std::filesystem::path path = tmp_dir / "image.inr.gz";
    auto my_writer                   = std::make_shared<io::itk::inr_image_writer>();
    my_writer->set_object(_image);
    my_writer->set_file(path);
    my_writer->write();

    // load image
    auto image2    = std::make_shared<data::image>();
    auto my_reader = std::make_shared<io::itk::inr_image_reader>();
    my_reader->set_object(image2);
    my_reader->set_file(path);
    my_reader->read();

    io::itk::ut::helper::round_spacing(image2);

    image2->set_window_center(_image->window_center());
    image2->set_window_width(_image->window_width());

    // check image
    // inr only support float spacing and float origin => add tolerance for comparison (+/-0.00001)
    CPPUNIT_ASSERT(*_image == *image2);
}

//------------------------------------------------------------------------------

} // namespace sight::io::itk::ut
