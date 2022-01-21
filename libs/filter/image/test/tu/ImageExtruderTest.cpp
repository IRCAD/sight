/************************************************************************
 *
 * Copyright (C) 2020-2022 IRCAD France
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

#include "ImageExtruderTest.hpp"

#include <filter/image/ImageExtruder.hpp>

CPPUNIT_TEST_SUITE_REGISTRATION(sight::filter::image::ut::ImageExtruderTest);

namespace sight::filter::image
{

namespace ut
{

//------------------------------------------------------------------------------

void ImageExtruderTest::setUp()
{
    m_image = data::Image::New();
    const auto dumpLock = m_image->lock();
    m_image->setSpacing(m_spacing);
    m_image->setOrigin(m_origin);
    m_image->resize(m_size, m_type, m_format);

    const auto iterEnd = m_image->end<std::int8_t>();
    for(auto iter = m_image->begin<std::int8_t>() ; iter != iterEnd ; ++iter)
    {
        *iter = std::numeric_limits<std::int8_t>::max();
    }
}

//------------------------------------------------------------------------------

void ImageExtruderTest::tearDown()
{
    m_image.reset();
}

//------------------------------------------------------------------------------

void ImageExtruderTest::extrudeTriangleMesh()
{
    // Create a cube.
    const data::Mesh::sptr mesh = data::Mesh::New();
    const auto lock             = mesh->lock();
    mesh->resize(8, 12, data::Mesh::CellType::TRIANGLE);

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
            it->pt[0] = indexes[pt++];
            it->pt[1] = indexes[pt++];
            it->pt[2] = indexes[pt++];

            ++it;
        }
    }

    const data::Image::sptr image = data::Image::New();
    image->deepCopy(m_image);

    filter::image::ImageExtruder::extrude(image, mesh);

    const auto dumpLock       = image->lock();
    const auto dumpOriginLock = m_image->lock();
    for(std::size_t z = 0 ; z < m_size[2] ; ++z)
    {
        for(std::size_t y = 0 ; y < m_size[1] ; ++y)
        {
            for(std::size_t x = 0 ; x < m_size[0] ; ++x)
            {
                if(x >= static_cast<float>(m_origin[0] + 1) && x < static_cast<float>(m_size[0] - 1)
                   && y >= static_cast<float>(m_origin[1] + 1) && y < static_cast<float>(m_size[1] - 1)
                   && z >= static_cast<float>(m_origin[2] + 1) && z < static_cast<float>(m_size[2] - 1))
                {
                    CPPUNIT_ASSERT_EQUAL(
                        std::numeric_limits<std::int8_t>::lowest(),
                        image->at<std::int8_t>(x, y, z)
                    );
                }
                else
                {
                    CPPUNIT_ASSERT_EQUAL(m_image->at<std::int8_t>(x, y, z), image->at<std::int8_t>(x, y, z));
                }
            }
        }
    }
}

//------------------------------------------------------------------------------

void ImageExtruderTest::extrudeQuadMesh()
{
    // Create a cube.
    const data::Mesh::sptr mesh = data::Mesh::New();
    const auto lock             = mesh->lock();

    mesh->resize(8, 6, data::Mesh::CellType::QUAD);

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
            it->pt[0] = indexes[pt++];
            it->pt[1] = indexes[pt++];
            it->pt[2] = indexes[pt++];
            it->pt[3] = indexes[pt++];

            ++it;
        }
    }

    const data::Image::sptr image = data::Image::New();
    image->deepCopy(m_image);

    filter::image::ImageExtruder::extrude(image, mesh);

    const auto dumpLock       = image->lock();
    const auto dumpOriginLock = m_image->lock();
    for(std::size_t z = 0 ; z < m_size[2] ; ++z)
    {
        for(std::size_t y = 0 ; y < m_size[1] ; ++y)
        {
            for(std::size_t x = 0 ; x < m_size[0] ; ++x)
            {
                if(x >= static_cast<float>(m_origin[0] + 1) && x < static_cast<float>(m_size[0] - 1)
                   && y >= static_cast<float>(m_origin[1] + 1) && y < static_cast<float>(m_size[1] - 1)
                   && z >= static_cast<float>(m_origin[2] + 1) && z < static_cast<float>(m_size[2] - 1))
                {
                    CPPUNIT_ASSERT_EQUAL(
                        std::numeric_limits<std::int8_t>::lowest(),
                        image->at<std::int8_t>(x, y, z)
                    );
                }
                else
                {
                    CPPUNIT_ASSERT_EQUAL(m_image->at<std::int8_t>(x, y, z), image->at<std::int8_t>(x, y, z));
                }
            }
        }
    }
}

//------------------------------------------------------------------------------

} // namespace ut.

} // namespace sight::filter::image.
