/************************************************************************
 *
 * Copyright (C) 2014-2024 IRCAD France
 * Copyright (C) 2014-2021 IHU Strasbourg
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

#include "layer_test.hpp"

#include <viz/scene3d/layer.hpp>

#include <cstdint>

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION(sight::viz::scene3d::helper::ut::layer_test);

namespace sight::viz::scene3d::helper::ut
{

//------------------------------------------------------------------------------

void layer_test::setUp()
{
}

//------------------------------------------------------------------------------

void layer_test::tearDown()
{
}

//------------------------------------------------------------------------------

void layer_test::set_orthographic()
{
    auto layer = std::make_shared<sight::viz::scene3d::layer>();

    CPPUNIT_ASSERT(!layer->is_orthographic_camera_force());

    layer->set_orthographic_camera(true);

    CPPUNIT_ASSERT(layer->is_orthographic_camera_force());
}

//------------------------------------------------------------------------------

} // namespace sight::viz::scene3d::helper::ut
