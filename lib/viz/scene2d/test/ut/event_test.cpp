/************************************************************************
 *
 * Copyright (C) 2026 IRCAD France
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

#include <viz/scene2d/data/event.hpp>

#include <doctest/doctest.h>

TEST_CASE("event_getters_setters")
{
    sight::viz::scene2d::data::event event;

    // Accepted
    CHECK_FALSE(event.is_accepted());
    event.set_accepted(true);
    CHECK(event.is_accepted());

    // Coord
    event.set_coord({10., 20.});
    const auto& coord = event.get_coord();
    CHECK(coord.x == 10.);
    CHECK(coord.y == 20.);

    // Old size
    event.set_old_size({640., 480.});
    CHECK(event.get_old_size().x == 640.);
    CHECK(event.get_old_size().y == 480.);

    // Size
    event.set_size({800., 600.});
    CHECK(event.size().x == 800.);
    CHECK(event.size().y == 600.);

    // Type
    event.set_type(sight::viz::scene2d::data::event::resize);
    CHECK(event.type() == sight::viz::scene2d::data::event::resize);
}
