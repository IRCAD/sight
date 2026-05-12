/************************************************************************
 *
 * Copyright (C) 2023-2026 IRCAD France
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

#include "helper.hpp"

#include <core/os/temp_path.hpp>

#include <io/itk/inr_image_reader.hpp>
#include <io/itk/inr_image_writer.hpp>
#include <io/itk/jpg_image_writer.hpp>
#include <io/itk/nifti.hpp>

#include <utest_data/data.hpp>
#include <utest_data/generator/image.hpp>

#include <doctest/doctest.h>

static const double EPSILON = 0.00001;

//------------------------------------------------------------------------------

static void inr_read_write_check(sight::data::image::sptr _image)
{
    const sight::data::image::origin_t origin = {0., 0., 0.};
    _image->set_origin(origin);

    sight::core::os::temp_dir tmp_dir;
    const std::filesystem::path path = tmp_dir / "image.inr.gz";
    auto my_writer                   = std::make_shared<sight::io::itk::inr_image_writer>();
    my_writer->set_object(_image);
    my_writer->set_file(path);
    auto write_observer = std::make_shared<sight::core::progress::observer>("Test write");
    my_writer->write(write_observer);

    auto image2    = std::make_shared<sight::data::image>();
    auto my_reader = std::make_shared<sight::io::itk::inr_image_reader>();
    my_reader->set_object(image2);
    my_reader->set_file(path);
    auto read_observer = std::make_shared<sight::core::progress::observer>("Test read");
    my_reader->read(read_observer);

    sight::io::itk::ut::helper::round_spacing(image2);

    image2->set_window_center(_image->window_center());
    image2->set_window_width(_image->window_width());

    CHECK(*_image == *image2);
}

//------------------------------------------------------------------------------

static void inr_stress_test_with_type(sight::core::type _type, int _nb_test)
{
    for(int nb = 0 ; nb < _nb_test ; ++nb)
    {
        sight::data::image::sptr image = std::make_shared<sight::data::image>();
        sight::utest_data::generator::image::generate_random_image(image, _type);

        image->set_orientation({1.F, 0.F, 0.F, 0.F, 1.F, 0.F, 0.F, 0.F, 1.F});

        inr_read_write_check(image);
    }
}

//------------------------------------------------------------------------------

static void nifti_read_check(
    const sight::data::image::sptr& _image_to_test,
    const sight::core::type& _expected_type,
    const std::size_t _expected_dim,
    const sight::data::image::spacing_t& _expected_spacing,
    const sight::data::image::origin_t& _expected_origin,
    const sight::data::image::size_t& _expected_size,
    const sight::data::image::orientation_t& _expected_orientation
)
{
    CHECK_EQ(_image_to_test->type(), _expected_type);

    CHECK_EQ(
        static_cast<std::size_t>(_image_to_test->num_dimensions()),
        _expected_dim
    );

    for(std::size_t i = 0 ; i < _expected_dim ; ++i)
    {
        CHECK_EQ(
            static_cast<sight::data::image::spacing_t::value_type>(_image_to_test->spacing()[i]),
            doctest::Approx(static_cast<sight::data::image::spacing_t::value_type>(_expected_spacing[i])).epsilon(
                EPSILON
            )
        );
        CHECK_EQ(
            static_cast<sight::data::image::origin_t::value_type>(_image_to_test->origin()[i]),
            doctest::Approx(static_cast<sight::data::image::origin_t::value_type>(_expected_origin[i])).epsilon(EPSILON)
        );
        CHECK_EQ(
            static_cast<sight::data::image::size_t::value_type>(_image_to_test->size()[i]),
            static_cast<sight::data::image::size_t::value_type>(_expected_size[i])
        );
    }

    const auto& actual_orientation = _image_to_test->orientation();

    for(std::size_t i = 0 ; i < _expected_orientation.size() ; ++i)
    {
        CHECK_EQ(_expected_orientation[i], doctest::Approx(actual_orientation[i]).epsilon(EPSILON));
    }
}

TEST_SUITE("sight::io::itk::image_reader_writer")
{
    TEST_CASE("inr_read_write")
    {
        auto image = std::make_shared<sight::data::image>();
        sight::utest_data::generator::image::generate_random_image(image, sight::core::type::INT16);

        image->set_orientation({1.F, 0.F, 0.F, 0.F, 1.F, 0.F, 0.F, 0.F, 1.F});

        inr_read_write_check(image);
    }

    TEST_CASE("inr_stress")
    {
        sight::core::type type = sight::core::type::UINT8;
        inr_stress_test_with_type(type, 5);

        type = sight::core::type::INT16;
        inr_stress_test_with_type(type, 5);

        type = sight::core::type::UINT16;
        inr_stress_test_with_type(type, 5);

        type = sight::core::type::INT32;
        inr_stress_test_with_type(type, 5);

        type = sight::core::type::UINT32;
        inr_stress_test_with_type(type, 5);

        type = sight::core::type::FLOAT32;
        inr_stress_test_with_type(type, 5);
    }

    TEST_CASE("nifti_read")
    {
        // cspell:ignore Dkidney
        const std::filesystem::path sight_image_path(sight::utest_data::dir() / "sight/image/nii/3Dkidney.nii");

        CHECK_MESSAGE(
            std::filesystem::exists(sight_image_path),
            "The file '",
            sight_image_path.string(),
            "' does not exist"
        );

        auto sight_image = std::make_shared<sight::data::image>();
        CHECK_EQ(sight::io::itk::read_nifti_image("/somewhere/wrong_path.nii", sight_image), false);
        CHECK_EQ(sight::io::itk::read_nifti_image(sight_image_path, sight_image), true);

        nifti_read_check(
            sight_image,
            sight::core::type::UINT8,
            3,
            {0.744924, 0.744924, 0.744924},
            {0, 0, 0},
            {481, 362, 478},
        {
            1, 0, 0,
            0, 1, 0,
            0, 0, 1
        });

        const std::filesystem::path external_image_path(sight::utest_data::dir() / "sight/image/nii/brain.nii");

        CHECK_MESSAGE(
            std::filesystem::exists(external_image_path),
            "The file '",
            external_image_path.string(),
            "' does not exist"
        );

        sight::data::image::sptr external_image = std::make_shared<sight::data::image>();
        sight::io::itk::read_nifti_image(external_image_path, external_image);

        nifti_read_check(
            external_image,
            sight::core::type::UINT8,
            3,
            {0.5, 0.5, 0.5},
            {75, 107, -69.5},
            {301, 370, 316},
        {
            -1, 0, 0,
            0, -1, 0,
            0, 0, 1
        });
    }

    TEST_CASE("nifti_write")
    {
        sight::data::image::sptr image = std::make_shared<sight::data::image>();
        sight::utest_data::generator::image::generate_random_image(image, sight::core::type::INT16);

        const sight::data::image::spacing_t spacing_d = {0.5F, 0.001F, 1.25F};
        const sight::data::image::origin_t origin_d   = {0.5F, 0.25F, 0.25F};
        image->set_spacing(spacing_d);
        image->set_origin(origin_d);

        const sight::data::image::orientation_t orientation = {0.36F, 0.48F, -0.8F, -0.8F, 0.6F, 0.0F, 0.48F, 0.64F,
                                                               0.6F
        };
        image->set_orientation(orientation);

        sight::io::itk::ut::helper::round_orientation(image);

        CHECK_EQ(sight::io::itk::write_nifti_image("/somewhere/wrong_path.nii", image), false);

        sight::core::os::temp_dir tmp_dir;
        const std::filesystem::path filename = tmp_dir / "image.nii";
        CHECK_EQ(sight::io::itk::write_nifti_image(filename, image), true);

        CHECK_MESSAGE(
            std::filesystem::exists(filename),
            "test on '",
            filename.string(),
            "' failed "
        );

        sight::data::image::sptr image2 = std::make_shared<sight::data::image>();
        image2->set_window_center(image->window_center());
        image2->set_window_width(image->window_width());

        sight::io::itk::read_nifti_image(filename, image2);

        sight::io::itk::ut::helper::round_orientation(image2);

        CHECK(*image == *image2);
    }

    TEST_CASE("jpeg_write")
    {
        sight::data::image::sptr image = std::make_shared<sight::data::image>();
        sight::utest_data::generator::image::generate_random_image(image, sight::core::type::INT16);

        sight::core::os::temp_dir tmp_dir;
        auto my_writer = std::make_shared<sight::io::itk::jpg_image_writer>();
        my_writer->set_object(image);
        my_writer->set_folder(tmp_dir);
        auto write_observer = std::make_shared<sight::core::progress::observer>("Test write");
        CHECK_NOTHROW(my_writer->write(write_observer));
    }

    TEST_CASE("inr_read_jpeg_write")
    {
        std::filesystem::path path_inr = sight::utest_data::dir() / "sight" / "image" / "inr" / "image.inr.gz";

        CHECK_MESSAGE(
            std::filesystem::exists(path_inr),
            "The file '",
            path_inr.string(),
            "' does not exist"
        );

        sight::data::image::sptr image                   = std::make_shared<sight::data::image>();
        sight::io::itk::inr_image_reader::sptr my_reader = std::make_shared<sight::io::itk::inr_image_reader>();
        my_reader->set_object(image);
        my_reader->set_file(path_inr);
        auto read_observer = std::make_shared<sight::core::progress::observer>("Test read");
        my_reader->read(read_observer);

        sight::core::os::temp_dir tmp_dir;
        auto my_writer = std::make_shared<sight::io::itk::jpg_image_writer>();
        my_writer->set_object(image);
        my_writer->set_folder(tmp_dir);
        auto write_observer = std::make_shared<sight::core::progress::observer>("Test write");
        my_writer->write(write_observer);
    }
} // TEST_SUITE
