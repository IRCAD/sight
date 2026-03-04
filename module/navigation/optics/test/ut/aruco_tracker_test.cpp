/************************************************************************
 *
 * Copyright (C) 2025-2026 IRCAD France
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

#include <data/camera.hpp>
#include <data/image.hpp>
#include <data/marker_map.hpp>

#include <service/op.hpp>

#include <boost/property_tree/xml_parser.hpp>

#include <doctest/doctest.h>

TEST_SUITE("sight::module::navigation::optics")
{
    TEST_CASE("aruco_tracker")
    {
        auto srv = sight::service::add("sight::module::navigation::optics::aruco_tracker");

        sight::data::camera::sptr camera         = std::make_shared<sight::data::camera>();
        sight::data::image::sptr frame           = std::make_shared<sight::data::image>();
        sight::data::marker_map::sptr marker_map = std::make_shared<sight::data::marker_map>();

        srv->set_input(camera, "camera");
        srv->set_inout(frame, "frame");
        srv->set_inout(marker_map, "marker_map", true, true, 0);

        std::stringstream config_string;
        config_string << "<track>"
        << "<marker id=\"0,1,2,3,4,5,6,7,8,42,100,101,102,103,104,105\" />"
        << "</track>";
        sight::service::base::config_t config;
        boost::property_tree::read_xml(config_string, config);
        srv->set_config(config);

        CHECK_NOTHROW(srv->configure());
        CHECK_NOTHROW(srv->start().get());
        CHECK_NOTHROW(srv->update().get());
        CHECK_NOTHROW(srv->stop().get());

        sight::service::remove(srv);
    }
}
