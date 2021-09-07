/************************************************************************
 *
 * Copyright (C) 2020-2021 IRCAD France
 * Copyright (C) 2017 IHU Strasbourg
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

#include <atoms/Map.hpp>

#include <cppunit/extensions/HelperMacros.h>

namespace sight::io::patch::semantic
{

namespace ut
{

/**
 * @brief Test patch to convert 'm_imageLandmarksId' field  of 'data::Image' from 'data::PointList' to
 *        `data::Landmarks`.
 */
class ImageV10ToV09ALATest : public CPPUNIT_NS::TestFixture
{
CPPUNIT_TEST_SUITE(ImageV10ToV09ALATest);
CPPUNIT_TEST(applyPatchTest);
CPPUNIT_TEST_SUITE_END();

public:

    // interface
    void setUp();
    void tearDown();

    void applyPatchTest();

    void addPoint(sight::atoms::Map::sptr map, const std::array<double, 3>& pt, const std::string& label);
};

} //namespace ut

} //namespace sight::io::patch::semantic
