/************************************************************************
 *
 * Copyright (C) 2009-2026 IRCAD France
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

#include "helper.hpp"

#include <core/memory/buffer_manager.hpp>
#include <core/tools/random/generator.hpp>

#include <io/itk/itk.hpp>

#include <itkImage.h>

#include <utest_data/generator/image.hpp>

#include <doctest/doctest.h>

#include <cstddef>
#include <cstdint>

using sight::core::tools::random::safe_rand;

//-----------------------------------------------------------------------------

template<class TYPE>
static void stress_test_for_a_type()
{
    for(unsigned char k = 0 ; k < 5 ; k++)
    {
        auto image = std::make_shared<sight::data::image>();
        sight::utest_data::generator::image::generate_random_image(image, sight::core::type::get<TYPE>());

        constexpr sight::data::image::orientation_t orientation = {0.36, 0.48, -0.8, -0.8, 0.6, 0.0, 0.48, 0.64, 0.6};
        image->set_orientation(orientation);

        using image_t = ::itk::Image<TYPE, 3>;
        typename image_t::Pointer itk_image = sight::io::itk::move_to_itk<image_t>(image);

        sight::data::image::sptr image2 = std::make_shared<sight::data::image>();
        sight::io::itk::move_from_itk<image_t>(itk_image, *image2, false);

        image2->set_window_center(image->window_center());
        image2->set_window_width(image->window_width());

        CHECK(*image == *image2);

        sight::data::image::sptr image3 = sight::io::itk::move_from_itk<image_t>(itk_image, false);
        image3->set_window_center(image->window_center());
        image3->set_window_width(image->window_width());

        CHECK(*image == *image3);
    }
}

/// Takes the buffer of an itk::Image, then dumps, restores and resizes it: each step reallocates it with its policy,
/// which must keep its content and its size.
template<class TYPE>
static void check_taken_buffer()
{
    using image_t = ::itk::Image<TYPE, 3>;

    const auto itk_image = image_t::New();
    typename image_t::SizeType size;
    size.Fill(4);
    itk_image->SetRegions(size);
    itk_image->Allocate();
    constexpr std::size_t element_count = 64;
    for(std::size_t index = 0 ; index < element_count ; ++index)
    {
        itk_image->GetBufferPointer()[index] = static_cast<TYPE>(index);
    }

    const auto check_content =
        [](const sight::data::image& _image, std::size_t _count)
        {
            const auto* const buffer = static_cast<const TYPE*>(_image.buffer());
            for(std::size_t index = 0 ; index < _count ; ++index)
            {
                REQUIRE_EQ(buffer[index], static_cast<TYPE>(index));
            }
        };

    sight::data::image image;
    sight::io::itk::move_from_itk<image_t>(itk_image, image, true);
    {
        const auto lock = image.dump_lock();
        check_content(image, element_count);
    }

    // The buffer manager destroys it and allocates another one, through the same policy.
    const auto manager = sight::core::memory::buffer_manager::get();
    manager->dump_buffer(image.get_buffer_object()->get_buffer_pointer()).get();
    {
        const auto lock = image.dump_lock();
        check_content(image, element_count);
    }

    const auto type = sight::core::type::get<TYPE>();
    {
        const auto lock = image.dump_lock();
        image.resize({8, 8, 8}, type, sight::data::image::gray_scale);
        CHECK_EQ(image.get_buffer_object()->size(), 512 * sizeof(TYPE));
        check_content(image, element_count);

        image.resize({2, 2, 2}, type, sight::data::image::gray_scale);
        CHECK_EQ(image.get_buffer_object()->size(), 8 * sizeof(TYPE));
        check_content(image, 8);
    }

    // After a resize too, the policy still knows the size it has to keep.
    manager->dump_buffer(image.get_buffer_object()->get_buffer_pointer()).get();
    {
        const auto lock = image.dump_lock();
        check_content(image, 8);
        image.resize({3, 3, 3}, type, sight::data::image::gray_scale);
        check_content(image, 8);
    }
}

//-----------------------------------------------------------------------------

TEST_SUITE("sight::io::itk::image_conversion")
{
    TEST_CASE("resizes_dumps_and_frees_a_buffer_taken_from_itk")
    {
        check_taken_buffer<std::uint8_t>();
        check_taken_buffer<std::int16_t>();
        check_taken_buffer<float>();
    }

    //-------------------------------------------------------------------------

    TEST_CASE("conversion")
    {
        // create Image
        sight::data::image::sptr image = std::make_shared<sight::data::image>();
        sight::utest_data::generator::image::generate_random_image(image, sight::core::type::INT16);
        const sight::data::image::orientation_t orientation = {0.36, 0.48, -0.8, -0.8, 0.6, 0.0, 0.48, 0.64, 0.6};
        image->set_orientation(orientation);

        using image_t = ::itk::Image<std::int16_t, 3>;
        image_t::Pointer itk_image      = sight::io::itk::move_to_itk<image_t>(image);
        sight::data::image::sptr image2 = std::make_shared<sight::data::image>();
        sight::io::itk::move_from_itk<image_t>(itk_image, *image2, false);

        sight::io::itk::ut::helper::round_spacing(image);
        sight::io::itk::ut::helper::round_spacing(image2);

        image2->set_window_center(image->window_center());
        image2->set_window_width(image->window_width());

        CHECK(*image == *image2);

        sight::data::image::sptr image3 = sight::io::itk::move_from_itk<image_t>(itk_image, false);
        image3->set_window_center(image->window_center());
        image3->set_window_width(image->window_width());

        CHECK(*image == *image3);
    }

    TEST_CASE("stress")
    {
        stress_test_for_a_type<std::int8_t>();
        stress_test_for_a_type<std::uint8_t>();

        stress_test_for_a_type<std::int16_t>();
        stress_test_for_a_type<std::uint16_t>();

        stress_test_for_a_type<std::int32_t>();
        stress_test_for_a_type<std::uint32_t>();

        stress_test_for_a_type<std::int64_t>();
        stress_test_for_a_type<std::uint64_t>();

        stress_test_for_a_type<float>();
        stress_test_for_a_type<double>();
    }

    TEST_CASE("conversion_2d")
    {
        // create Image
        sight::data::image::sptr image  = std::make_shared<sight::data::image>();
        sight::data::image::size_t size = {
            static_cast<std::size_t>(safe_rand() % 100 + 2), static_cast<std::size_t>(safe_rand() % 100 + 2), 0
        };
        sight::data::image::spacing_t spacing = {(safe_rand() % 200 + 1) / 100., (safe_rand() % 200 + 1) / 100., 0.
        };
        sight::data::image::origin_t origin = {(safe_rand() % 200 - 100) / 3., (safe_rand() % 200 - 100) / 3., 0.
        };
        sight::data::image::orientation_t orientation = {0.36F, 0.48F, 0.0F, -0.8F, 0.6F, 0.0F, 0.0F, 0.0F, 1.0F};
        sight::core::type type                        = sight::core::type::INT16;

        sight::utest_data::generator::image::generate_image(
            image,
            size,
            spacing,
            origin,
            orientation,
            type,
            sight::data::image::gray_scale,
            0
        );

        using image_t = ::itk::Image<std::int16_t, 2>;

        image_t::Pointer itk_image = sight::io::itk::move_to_itk<image_t>(image);

        sight::data::image::sptr image2 = std::make_shared<sight::data::image>();
        bool image2_manages_his_buffer  = false;
        sight::io::itk::move_from_itk<image_t>(itk_image, *image2, image2_manages_his_buffer);
        sight::io::itk::ut::helper::round_spacing(image);
        sight::io::itk::ut::helper::round_spacing(image2);

        image2->set_window_center(image->window_center());
        image2->set_window_width(image->window_width());

        CHECK(*image == *image2);

        sight::data::image::sptr image3 = sight::io::itk::move_from_itk<image_t>(itk_image, false);
        image3->set_window_center(image->window_center());
        image3->set_window_width(image->window_width());

        CHECK(*image == *image3);
    }
} // TEST_SUITE
