/************************************************************************
 *
 * Copyright (C) 2009-2026 IRCAD France
 * Copyright (C) 2012-2020 IHU Strasbourg
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

#include <service/op.hpp>

#include <viz/scene2d/data/parser/viewport.hpp>
#include <viz/scene2d/data/viewport.hpp>

#include <boost/property_tree/xml_parser.hpp>

#include <doctest/doctest.h>

//------------------------------------------------------------------------------

TEST_SUITE("sight::viz::scene2d::viewport")
{
    TEST_CASE("parser")
    {
        sight::service::config_t config;
        std::stringstream config_string(R"(<config x="-700" y="0.1" width="100" height="1.3"/>)");
        boost::property_tree::read_xml(config_string, config);

        auto parser =
            sight::service::add<sight::viz::scene2d::data::parser::viewport>(
                "sight::viz::scene2d::data::parser::viewport"
            );
        CHECK(parser->is_a("sight::viz::scene2d::data::parser::viewport"));

        auto viewport = std::make_shared<sight::viz::scene2d::data::viewport>();

        sight::service::object_parser::objects_t sub_objects;
        parser->parse(config, viewport, sub_objects);

        CHECK(viewport->x() == doctest::Approx(-700.).epsilon(0.00001));
        CHECK(viewport->y() == doctest::Approx(.1).epsilon(0.00001));
        CHECK(viewport->width() == doctest::Approx(100.).epsilon(0.00001));
        CHECK(viewport->height() == doctest::Approx(1.3).epsilon(0.00001));
    }
}
