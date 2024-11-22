/************************************************************************
 *
 * Copyright (C) 2020-2024 IRCAD France
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

#pragma once

#include <core/type.hpp>

#include <data/image.hpp>
#include <data/mesh.hpp>

#include <cppunit/extensions/HelperMacros.h>

namespace sight::filter::image::ut
{

class image_extruder_test : public CPPUNIT_NS::TestFixture
{
CPPUNIT_TEST_SUITE(image_extruder_test);
CPPUNIT_TEST(extrude_triangle_mesh);
CPPUNIT_TEST(extrude_quad_mesh);
CPPUNIT_TEST_SUITE_END();

public:

    void setUp() override;

    void tearDown() override;

    void extrude_triangle_mesh();

    void extrude_quad_mesh();

private:

    const core::type m_type {core::type::INT8};

    const enum data::image::pixel_format_t m_format
    {
        data::image::pixel_format_t::gray_scale
    };
    const data::image::size_t m_size {8, 16, 24};
    const data::image::origin_t m_origin {0.F, 0.F, 0.F};
    const data::image::spacing_t m_spacing {1.F, 1.F, 1.F};

    data::image::sptr m_image;
};

} // namespace sight::filter::image::ut
