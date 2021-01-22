/************************************************************************
 *
 * Copyright (C) 2020-2021 IRCAD France
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

#include <core/tools/Type.hpp>

#include <fwData/Image.hpp>
#include <fwData/Mesh.hpp>

#include <cppunit/extensions/HelperMacros.h>

namespace imageFilterOp
{
namespace ut
{

class ImageExtruderTest : public CPPUNIT_NS::TestFixture
{

CPPUNIT_TEST_SUITE(ImageExtruderTest);
CPPUNIT_TEST(extrudeTriangleMesh);
CPPUNIT_TEST(extrudeQuadMesh);
CPPUNIT_TEST_SUITE_END();

public:

    void setUp() override;

    void tearDown() override;

    void extrudeTriangleMesh();

    void extrudeQuadMesh();

private:

    const core::tools::Type m_type { "int8" };

    const ::fwData::Image::PixelFormat m_format { ::fwData::Image::PixelFormat::GRAY_SCALE };

    const ::fwData::Image::Size m_size { 8, 16, 24 };

    const ::fwData::Image::Origin m_origin { 0.f, 0.f, 0.f };

    const ::fwData::Image::Spacing m_spacing { 1.f, 1.f, 1.f };

    ::fwData::Image::sptr m_image;

};

} // namespace ut.
} // namespace imageFilterOp.
