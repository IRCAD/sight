/************************************************************************
 *
 * Copyright (C) 2020-2025 IRCAD France
 * Copyright (C) 2020 IHU Strasbourg
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

#include <core/type.hpp>

#include <data/image.hpp>
#include <data/mesh.hpp>

#include <filter/image/image_extruder.hpp>

#include <doctest/doctest.h>

TEST_SUITE("sight::filter::image::image_extruder")
{
// Test constants
    static const sight::core::type TYPE                = sight::core::type::INT8;
    static const auto FORMAT                           = sight::data::image::pixel_format_t::gray_scale;
    static const sight::data::image::size_t SIZE       = {8, 16, 24};
    static const sight::data::image::origin_t ORIGIN   = {0.F, 0.F, 0.F};
    static const sight::data::image::spacing_t SPACING = {1.F, 1.F, 1.F};

//------------------------------------------------------------------------------

    static sight::data::image::sptr create_test_image()
    {
        sight::data::image::sptr image = std::make_shared<sight::data::image>();
        const auto dump_lock           = image->dump_lock();

        image->set_spacing(SPACING);
        image->set_origin(ORIGIN);
        image->resize(SIZE, TYPE, FORMAT);

        const auto iter_end = image->end<std::int8_t>();

        for(auto iter = image->begin<std::int8_t>() ; iter != iter_end ; ++iter)
        {
            *iter = std::numeric_limits<std::int8_t>::max();
        }

        return image;
    }

//------------------------------------------------------------------------------

    TEST_CASE("extrude_triangle_mesh")
    {
        sight::data::image::sptr m_image = create_test_image();

        // Create a cube.
        const sight::data::mesh::sptr mesh = std::make_shared<sight::data::mesh>();
        const auto lock                    = mesh->dump_lock();

        mesh->resize(8, 12, sight::data::mesh::cell_type_t::triangle);

        {
            auto it = mesh->begin<sight::data::iterator::point::xyz>();

            it->x = static_cast<float>(ORIGIN[0] + 1);
            it->y = static_cast<float>(ORIGIN[1] + 1);
            it->z = static_cast<float>(ORIGIN[2] + 1);
            ++it;

            it->x = static_cast<float>(ORIGIN[0] + 1);
            it->y = static_cast<float>(SIZE[1] - 1);
            it->z = static_cast<float>(ORIGIN[2] + 1);
            ++it;

            it->x = static_cast<float>(SIZE[0] - 1);
            it->y = static_cast<float>(SIZE[1] - 1);
            it->z = static_cast<float>(ORIGIN[2] + 1);
            ++it;

            it->x = static_cast<float>(SIZE[0] - 1);
            it->y = static_cast<float>(ORIGIN[1] + 1);
            it->z = static_cast<float>(ORIGIN[2] + 1);
            ++it;

            it->x = static_cast<float>(ORIGIN[0] + 1);
            it->y = static_cast<float>(ORIGIN[1] + 1);
            it->z = static_cast<float>(SIZE[2] - 1);
            ++it;

            it->x = static_cast<float>(ORIGIN[0] + 1);
            it->y = static_cast<float>(SIZE[1] - 1);
            it->z = static_cast<float>(SIZE[2] - 1);
            ++it;

            it->x = static_cast<float>(SIZE[0] - 1);
            it->y = static_cast<float>(SIZE[1] - 1);
            it->z = static_cast<float>(SIZE[2] - 1);
            ++it;

            it->x = static_cast<float>(SIZE[0] - 1);
            it->y = static_cast<float>(ORIGIN[1] + 1);
            it->z = static_cast<float>(SIZE[2] - 1);
            ++it;
        }

        {
            std::array<std::size_t, 36> indexes {
                0, 1, 2, 0, 2, 3,
                4, 5, 6, 4, 6, 7,
                1, 5, 4, 1, 4, 0,
                2, 3, 6, 3, 6, 7,
                1, 2, 6, 1, 6, 5,
                0, 3, 7, 0, 7, 4
            };

            auto it        = mesh->begin<sight::data::iterator::cell::triangle>();
            std::size_t pt = 0;

            for(std::size_t index = 0 ; index < 36 ; index += 3)
            {
                it->pt[0] = sight::data::iterator::cell_t(indexes[pt++]);
                it->pt[1] = sight::data::iterator::cell_t(indexes[pt++]);
                it->pt[2] = sight::data::iterator::cell_t(indexes[pt++]);
                ++it;
            }
        }

        const sight::data::image::sptr image = std::make_shared<sight::data::image>();
        image->resize(m_image->size(), sight::core::type::UINT8, sight::data::image::pixel_format_t::gray_scale);
        image->set_spacing(m_image->spacing());

        const auto dump_lock = image->dump_lock();
        std::fill(image->begin(), image->end(), std::uint8_t(255));

        sight::filter::image::image_extruder::extrude(image, mesh, nullptr);

        const auto dump_origin_lock = m_image->dump_lock();

        for(std::size_t z = 0 ; z < SIZE[2] ; ++z)
        {
            for(std::size_t y = 0 ; y < SIZE[1] ; ++y)
            {
                for(std::size_t x = 0 ; x < SIZE[0] ; ++x)
                {
                    if(double(x) >= ORIGIN[0] + 1 && x < SIZE[0] - 1
                       && double(y) >= ORIGIN[1] + 1 && y < SIZE[1] - 1
                       && double(z) >= ORIGIN[2] + 1 && z < SIZE[2] - 1)
                    {
                        CHECK_EQ(std::uint8_t(0), image->at<std::uint8_t>(x, y, z));
                    }
                    else
                    {
                        CHECK_EQ(std::uint8_t(255), image->at<std::uint8_t>(x, y, z));
                    }
                }
            }
        }
    }

//------------------------------------------------------------------------------

    TEST_CASE("extrude_quad_mesh")
    {
        sight::data::image::sptr m_image = create_test_image();

        // Create a cube.
        const sight::data::mesh::sptr mesh = std::make_shared<sight::data::mesh>();
        const auto lock                    = mesh->dump_lock();

        mesh->resize(8, 6, sight::data::mesh::cell_type_t::quad);

        {
            auto it = mesh->begin<sight::data::iterator::point::xyz>();

            it->x = static_cast<float>(ORIGIN[0] + 1);
            it->y = static_cast<float>(ORIGIN[1] + 1);
            it->z = static_cast<float>(ORIGIN[2] + 1);
            ++it;

            it->x = static_cast<float>(ORIGIN[0] + 1);
            it->y = static_cast<float>(SIZE[1] - 1);
            it->z = static_cast<float>(ORIGIN[2] + 1);
            ++it;

            it->x = static_cast<float>(SIZE[0] - 1);
            it->y = static_cast<float>(SIZE[1] - 1);
            it->z = static_cast<float>(ORIGIN[2] + 1);
            ++it;

            it->x = static_cast<float>(SIZE[0] - 1);
            it->y = static_cast<float>(ORIGIN[1] + 1);
            it->z = static_cast<float>(ORIGIN[2] + 1);
            ++it;

            it->x = static_cast<float>(ORIGIN[0] + 1);
            it->y = static_cast<float>(ORIGIN[1] + 1);
            it->z = static_cast<float>(SIZE[2] - 1);
            ++it;

            it->x = static_cast<float>(ORIGIN[0] + 1);
            it->y = static_cast<float>(SIZE[1] - 1);
            it->z = static_cast<float>(SIZE[2] - 1);
            ++it;

            it->x = static_cast<float>(SIZE[0] - 1);
            it->y = static_cast<float>(SIZE[1] - 1);
            it->z = static_cast<float>(SIZE[2] - 1);
            ++it;

            it->x = static_cast<float>(SIZE[0] - 1);
            it->y = static_cast<float>(ORIGIN[1] + 1);
            it->z = static_cast<float>(SIZE[2] - 1);
            ++it;
        }

        {
            std::array<std::size_t, 36> indexes {
                0, 1, 2, 3,
                4, 5, 6, 7,
                0, 1, 5, 4,
                2, 3, 7, 6,
                1, 2, 6, 5,
                0, 3, 7, 4
            };

            auto it        = mesh->begin<sight::data::iterator::cell::quad>();
            std::size_t pt = 0;

            for(std::size_t index = 0 ; index < 24 ; index += 4)
            {
                it->pt[0] = sight::data::iterator::cell_t(indexes[pt++]);
                it->pt[1] = sight::data::iterator::cell_t(indexes[pt++]);
                it->pt[2] = sight::data::iterator::cell_t(indexes[pt++]);
                it->pt[3] = sight::data::iterator::cell_t(indexes[pt++]);
                ++it;
            }
        }

        const sight::data::image::sptr image = std::make_shared<sight::data::image>();
        image->resize(m_image->size(), sight::core::type::UINT8, sight::data::image::pixel_format_t::gray_scale);
        image->set_spacing(m_image->spacing());

        const auto dump_lock = image->dump_lock();
        std::fill(image->begin(), image->end(), std::uint8_t(255));

        sight::filter::image::image_extruder::extrude(image, mesh, nullptr);

        const auto dump_origin_lock = m_image->dump_lock();

        for(std::size_t z = 0 ; z < SIZE[2] ; ++z)
        {
            for(std::size_t y = 0 ; y < SIZE[1] ; ++y)
            {
                for(std::size_t x = 0 ; x < SIZE[0] ; ++x)
                {
                    if(double(x) >= ORIGIN[0] + 1 && x < SIZE[0] - 1
                       && double(y) >= ORIGIN[1] + 1 && y < SIZE[1] - 1
                       && double(z) >= ORIGIN[2] + 1 && z < SIZE[2] - 1)
                    {
                        CHECK_EQ(std::uint8_t(0), image->at<std::uint8_t>(x, y, z));
                    }
                    else
                    {
                        CHECK_EQ(std::uint8_t(255), image->at<std::uint8_t>(x, y, z));
                    }
                }
            }
        }
    }

//------------------------------------------------------------------------------
} // TEST_SUITE("sight::filter::image::image_extruder")
