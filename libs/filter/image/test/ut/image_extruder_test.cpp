/************************************************************************
 *
 * Copyright (C) 2020-2023 IRCAD France
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

#include "image_extruder_test.hpp"

#include <filter/image/image_extruder.hpp>

CPPUNIT_TEST_SUITE_REGISTRATION(sight::filter::image::ut::image_extruder_test);

namespace sight::filter::image::ut
{

//------------------------------------------------------------------------------

void image_extruder_test::setUp()
{
    m_image = std::make_shared<data::image>();
    const auto dump_lock = m_image->dump_lock();
    m_image->set_spacing(m_spacing);
    m_image->set_origin(m_origin);
    m_image->resize(m_size, m_type, m_format);

    const auto iter_end = m_image->end<std::int8_t>();
    for(auto iter = m_image->begin<std::int8_t>() ; iter != iter_end ; ++iter)
    {
        *iter = std::numeric_limits<std::int8_t>::max();
    }
}

//------------------------------------------------------------------------------

void image_extruder_test::tearDown()
{
    m_image.reset();
}

//------------------------------------------------------------------------------

void image_extruder_test::extrude_triangle_mesh()
{
    // Create a cube.
    const data::mesh::sptr mesh = std::make_shared<data::mesh>();
    const auto lock             = mesh->dump_lock();
    mesh->resize(8, 12, data::mesh::cell_type_t::triangle);

    {
        auto it = mesh->begin<data::iterator::point::xyz>();

        it->x = static_cast<float>(m_origin[0] + 1);
        it->y = static_cast<float>(m_origin[1] + 1);
        it->z = static_cast<float>(m_origin[2] + 1);
        ++it;
        it->x = static_cast<float>(m_origin[0] + 1);
        it->y = static_cast<float>(m_size[1] - 1);
        it->z = static_cast<float>(m_origin[2] + 1);
        ++it;
        it->x = static_cast<float>(m_size[0] - 1);
        it->y = static_cast<float>(m_size[1] - 1);
        it->z = static_cast<float>(m_origin[2] + 1);
        ++it;
        it->x = static_cast<float>(m_size[0] - 1);
        it->y = static_cast<float>(m_origin[1] + 1);
        it->z = static_cast<float>(m_origin[2] + 1);
        ++it;
        it->x = static_cast<float>(m_origin[0] + 1);
        it->y = static_cast<float>(m_origin[1] + 1);
        it->z = static_cast<float>(m_size[2] - 1);
        ++it;
        it->x = static_cast<float>(m_origin[0] + 1);
        it->y = static_cast<float>(m_size[1] - 1);
        it->z = static_cast<float>(m_size[2] - 1);
        ++it;
        it->x = static_cast<float>(m_size[0] - 1);
        it->y = static_cast<float>(m_size[1] - 1);
        it->z = static_cast<float>(m_size[2] - 1);
        ++it;
        it->x = static_cast<float>(m_size[0] - 1);
        it->y = static_cast<float>(m_origin[1] + 1);
        it->z = static_cast<float>(m_size[2] - 1);
        ++it;
    }

    {
        std::array<std::size_t, 36> indexes {0, 1, 2, 0, 2, 3, 4, 5, 6, 4, 6, 7, 1, 5, 4, 1, 4, 0,
                                             2, 3, 6, 3, 6, 7, 1, 2, 6, 1, 6, 5, 0, 3, 7, 0, 7, 4
        };
        auto it        = mesh->begin<data::iterator::cell::triangle>();
        std::size_t pt = 0;
        for(std::size_t index = 0 ; index < 36 ; index += 3)
        {
            it->pt[0] = data::iterator::cell_t(indexes[pt++]);
            it->pt[1] = data::iterator::cell_t(indexes[pt++]);
            it->pt[2] = data::iterator::cell_t(indexes[pt++]);

            ++it;
        }
    }

    const data::image::sptr image = std::make_shared<data::image>();

    image->resize(m_image->size(), core::type::UINT8, data::image::pixel_format::gray_scale);
    image->set_spacing(m_image->spacing());

    const auto dump_lock = image->dump_lock();
    std::fill(image->begin(), image->end(), std::uint8_t(255));

    filter::image::image_extruder::extrude(image, mesh, nullptr);

    const auto dump_origin_lock = m_image->dump_lock();

    for(std::size_t z = 0 ; z < m_size[2] ; ++z)
    {
        for(std::size_t y = 0 ; y < m_size[1] ; ++y)
        {
            for(std::size_t x = 0 ; x < m_size[0] ; ++x)
            {
                if(double(x) >= m_origin[0] + 1 && x < m_size[0] - 1
                   && double(y) >= m_origin[1] + 1 && y < m_size[1] - 1
                   && double(z) >= m_origin[2] + 1 && z < m_size[2] - 1)
                {
                    CPPUNIT_ASSERT_EQUAL(std::uint8_t(0), image->at<std::uint8_t>(x, y, z));
                }
                else
                {
                    CPPUNIT_ASSERT_EQUAL(std::uint8_t(255), image->at<std::uint8_t>(x, y, z));
                }
            }
        }
    }
}

//------------------------------------------------------------------------------

void image_extruder_test::extrude_quad_mesh()
{
    // Create a cube.
    const data::mesh::sptr mesh = std::make_shared<data::mesh>();
    const auto lock             = mesh->dump_lock();

    mesh->resize(8, 6, data::mesh::cell_type_t::quad);

    {
        auto it = mesh->begin<data::iterator::point::xyz>();

        it->x = static_cast<float>(m_origin[0] + 1);
        it->y = static_cast<float>(m_origin[1] + 1);
        it->z = static_cast<float>(m_origin[2] + 1);
        ++it;
        it->x = static_cast<float>(m_origin[0] + 1);
        it->y = static_cast<float>(m_size[1] - 1);
        it->z = static_cast<float>(m_origin[2] + 1);
        ++it;
        it->x = static_cast<float>(m_size[0] - 1);
        it->y = static_cast<float>(m_size[1] - 1);
        it->z = static_cast<float>(m_origin[2] + 1);
        ++it;
        it->x = static_cast<float>(m_size[0] - 1);
        it->y = static_cast<float>(m_origin[1] + 1);
        it->z = static_cast<float>(m_origin[2] + 1);
        ++it;
        it->x = static_cast<float>(m_origin[0] + 1);
        it->y = static_cast<float>(m_origin[1] + 1);
        it->z = static_cast<float>(m_size[2] - 1);
        ++it;
        it->x = static_cast<float>(m_origin[0] + 1);
        it->y = static_cast<float>(m_size[1] - 1);
        it->z = static_cast<float>(m_size[2] - 1);
        ++it;
        it->x = static_cast<float>(m_size[0] - 1);
        it->y = static_cast<float>(m_size[1] - 1);
        it->z = static_cast<float>(m_size[2] - 1);
        ++it;
        it->x = static_cast<float>(m_size[0] - 1);
        it->y = static_cast<float>(m_origin[1] + 1);
        it->z = static_cast<float>(m_size[2] - 1);
        ++it;
    }

    {
        std::array<std::size_t, 36> indexes {0, 1, 2, 3, 4, 5, 6, 7, 0, 1, 5, 4, 2, 3, 7, 6, 1, 2, 6, 5, 0, 3, 7, 4};
        auto it        = mesh->begin<data::iterator::cell::quad>();
        std::size_t pt = 0;
        for(std::size_t index = 0 ; index < 24 ; index += 4)
        {
            it->pt[0] = data::iterator::cell_t(indexes[pt++]);
            it->pt[1] = data::iterator::cell_t(indexes[pt++]);
            it->pt[2] = data::iterator::cell_t(indexes[pt++]);
            it->pt[3] = data::iterator::cell_t(indexes[pt++]);

            ++it;
        }
    }

    const data::image::sptr image = std::make_shared<data::image>();
    image->resize(m_image->size(), core::type::UINT8, data::image::pixel_format::gray_scale);
    image->set_spacing(m_image->spacing());

    const auto dump_lock = image->dump_lock();
    std::fill(image->begin(), image->end(), std::uint8_t(255));

    filter::image::image_extruder::extrude(image, mesh, nullptr);

    const auto dump_origin_lock = m_image->dump_lock();

    for(std::size_t z = 0 ; z < m_size[2] ; ++z)
    {
        for(std::size_t y = 0 ; y < m_size[1] ; ++y)
        {
            for(std::size_t x = 0 ; x < m_size[0] ; ++x)
            {
                if(double(x) >= m_origin[0] + 1 && x < m_size[0] - 1
                   && double(y) >= m_origin[1] + 1 && y < m_size[1] - 1
                   && double(z) >= m_origin[2] + 1 && z < m_size[2] - 1)
                {
                    CPPUNIT_ASSERT_EQUAL(std::uint8_t(0), image->at<std::uint8_t>(x, y, z));
                }
                else
                {
                    CPPUNIT_ASSERT_EQUAL(std::uint8_t(255), image->at<std::uint8_t>(x, y, z));
                }
            }
        }
    }
}

//------------------------------------------------------------------------------

} // namespace sight::filter::image::ut
