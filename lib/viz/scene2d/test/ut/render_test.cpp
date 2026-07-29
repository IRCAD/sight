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
#include <viz/scene2d/registry/adaptor.hpp>
#include <viz/scene2d/render.hpp>

#include <doctest/doctest.h>

TEST_CASE("dispatch_interaction")
{
    auto renderer = std::make_shared<sight::viz::scene2d::render>();

    auto& registry =
        sight::viz::scene2d::registry::get_adaptor_registry();

    const auto saved_registry = registry;
    registry.clear();

    sight::viz::scene2d::data::event event;

    CHECK_FALSE(event.is_accepted());

    renderer->dispatch_interaction(event);

    CHECK_FALSE(event.is_accepted());

    registry = saved_registry;
}
