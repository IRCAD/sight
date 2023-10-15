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
CPPUNIT_TEST(extrudeTriangleMesh);
CPPUNIT_TEST(extrudeQuadMesh);
CPPUNIT_TEST_SUITE_END();

public:

    void setUp() override;

    void tearDown() override;

    void extrudeTriangleMesh();

    void extrudeQuadMesh();

private:

    const core::type m_type {core::type::INT8};

    const data::image::PixelFormat m_format {data::image::PixelFormat::GRAY_SCALE};

    const data::image::Size m_size {8, 16, 24};

    const data::image::Origin m_origin {0.F, 0.F, 0.F};

    const data::image::Spacing m_spacing {1.F, 1.F, 1.F};

    data::image::sptr m_image;
};

} // namespace sight::filter::image::ut
