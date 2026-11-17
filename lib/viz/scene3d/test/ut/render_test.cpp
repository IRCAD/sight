/************************************************************************
 *
 * Copyright (C) 2014-2026 IRCAD France
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

#include <core/runtime/runtime.hpp>

#include <service/macros.hpp>
#include <service/op.hpp>

#include <viz/scene3d/render.hpp>

#include <doctest/doctest.h>

TEST_SUITE("sight::viz::scene3d::render")
{
    TEST_CASE("configure_render_mode")
    {
        SUBCASE("auto mode")
        {
            auto offscreen  = std::make_shared<sight::data::image>();
            auto render_srv = sight::service::add<sight::viz::scene3d::render>("sight::viz::scene3d::render");

            const std::string config =
                "<inout key='offscreen' uid='dummy' />"
                "<scene render_mode='auto'>"
                "<background color='#36393E' />"
                "<layer id='default' />"
                "</scene>";

            render_srv->set_config(config);
            render_srv->set_inout(offscreen, "offscreen");
            render_srv->configure();

            CHECK(render_srv->get_render_mode() == sight::viz::scene3d::render::render_mode::automatic);

            sight::service::remove(render_srv);
        }

        SUBCASE("manual_mode")
        {
            auto offscreen  = std::make_shared<sight::data::image>();
            auto render_srv = sight::service::add<sight::viz::scene3d::render>("sight::viz::scene3d::render");

            const std::string config =
                "<inout key='offscreen' uid='dummy' />"
                "<scene render_mode='manual'>"
                "<background color='#36393E' />"
                "<layer id='default' />"
                "</scene>";

            render_srv->set_config(config);
            render_srv->set_inout(offscreen, "offscreen");
            render_srv->configure();

            CHECK(render_srv->get_render_mode() == sight::viz::scene3d::render::render_mode::manual);

            sight::service::remove(render_srv);
        }

        SUBCASE("default_mode (no render_mode attribute)")
        {
            auto offscreen  = std::make_shared<sight::data::image>();
            auto render_srv = sight::service::add<sight::viz::scene3d::render>("sight::viz::scene3d::render");

            const std::string config =
                "<inout key='offscreen' uid='dummy' />"
                "<scene>"
                "<background color='#36393E' />"
                "<layer id='default' />"
                "</scene>";

            render_srv->set_config(config);
            render_srv->set_inout(offscreen, "offscreen");
            render_srv->configure();

            CHECK(render_srv->get_render_mode() == sight::viz::scene3d::render::render_mode::automatic);

            sight::service::remove(render_srv);
        }
    }

    TEST_CASE("configure_background")
    {
        SUBCASE("background_with_single_color")
        {
            auto offscreen  = std::make_shared<sight::data::image>();
            auto render_srv = sight::service::add<sight::viz::scene3d::render>("sight::viz::scene3d::render");

            const std::string config =
                "<inout key='offscreen' uid='dummy' />"
                "<scene>"
                "<background color='#FF0000' />"
                "<layer id='default' />"
                "</scene>";

            render_srv->set_config(config);
            render_srv->set_inout(offscreen, "offscreen");
            render_srv->configure();

            sight::service::remove(render_srv);
        }

        SUBCASE("background_with_gradient_colors")
        {
            auto offscreen  = std::make_shared<sight::data::image>();
            auto render_srv = sight::service::add<sight::viz::scene3d::render>("sight::viz::scene3d::render");

            const std::string config =
                "<inout key='offscreen' uid='dummy' />"
                "<scene>"
                "<background top_color='#FF0000' bottom_color='#0000FF' />"
                "<layer id='default' />"
                "</scene>";

            render_srv->set_config(config);
            render_srv->set_inout(offscreen, "offscreen");
            render_srv->configure();

            sight::service::remove(render_srv);
        }

        SUBCASE("background_with_scales")
        {
            auto offscreen  = std::make_shared<sight::data::image>();
            auto render_srv = sight::service::add<sight::viz::scene3d::render>("sight::viz::scene3d::render");

            const std::string config =
                "<inout key='offscreen' uid='dummy' />"
                "<scene>"
                "<background top_color='#FF0000' bottom_color='#0000FF' top_scale='0.3' bottom_scale='0.7' />"
                "<layer id='default' />"
                "</scene>";

            render_srv->set_config(config);
            render_srv->set_inout(offscreen, "offscreen");
            render_srv->configure();

            sight::service::remove(render_srv);
        }

        SUBCASE("background_with_material")
        {
            auto offscreen  = std::make_shared<sight::data::image>();
            auto render_srv = sight::service::add<sight::viz::scene3d::render>("sight::viz::scene3d::render");

            const std::string config =
                "<inout key='offscreen' uid='dummy' />"
                "<scene>"
                "<background material='CustomMaterial' />"
                "<layer id='default' />"
                "</scene>";

            render_srv->set_config(config);
            render_srv->set_inout(offscreen, "offscreen");
            render_srv->configure();

            sight::service::remove(render_srv);
        }
    }

    TEST_CASE("configure_layers")
    {
        SUBCASE("single layer basic configuration")
        {
            auto offscreen  = std::make_shared<sight::data::image>();
            auto render_srv = sight::service::add<sight::viz::scene3d::render>("sight::viz::scene3d::render");

            const std::string config =
                "<inout key='offscreen' uid='dummy' />"
                "<scene>"
                "<background color='#000000' />"
                "<layer id='default' />"
                "</scene>";

            render_srv->set_config(config);
            render_srv->set_inout(offscreen, "offscreen");
            render_srv->configure();

            sight::service::remove(render_srv);
        }

        SUBCASE("multiple_layers")
        {
            auto offscreen  = std::make_shared<sight::data::image>();
            auto render_srv = sight::service::add<sight::viz::scene3d::render>("sight::viz::scene3d::render");

            const std::string config =
                "<inout key='offscreen' uid='dummy' />"
                "<scene>"
                "<background color='#000000' />"
                "<layer id='layer1' />"
                "<layer id='layer2' />"
                "<layer id='layer3' />"
                "</scene>";

            render_srv->set_config(config);
            render_srv->set_inout(offscreen, "offscreen");
            render_srv->configure();

            sight::service::remove(render_srv);
        }

        SUBCASE("layer_with_transparency")
        {
            auto offscreen  = std::make_shared<sight::data::image>();
            auto render_srv = sight::service::add<sight::viz::scene3d::render>("sight::viz::scene3d::render");

            const std::string config =
                "<inout key='offscreen' uid='dummy' />"
                "<scene>"
                "<background color='#000000' />"
                "<layer id='default' transparency='DepthPeeling' numPeels='8' />"
                "</scene>";

            render_srv->set_config(config);
            render_srv->set_inout(offscreen, "offscreen");
            render_srv->configure();

            sight::service::remove(render_srv);
        }

        SUBCASE("layer_with_stereo_mode")
        {
            auto offscreen  = std::make_shared<sight::data::image>();
            auto render_srv = sight::service::add<sight::viz::scene3d::render>("sight::viz::scene3d::render");

            const std::string config =
                "<inout key='offscreen' uid='dummy' />"
                "<scene>"
                "<background color='#000000' />"
                "<layer id='default' stereoMode='AutoStereo5' />"
                "</scene>";

            render_srv->set_config(config);
            render_srv->set_inout(offscreen, "offscreen");
            render_srv->configure();

            sight::service::remove(render_srv);
        }

        SUBCASE("layer_with_viewport configuration")
        {
            auto offscreen  = std::make_shared<sight::data::image>();
            auto render_srv = sight::service::add<sight::viz::scene3d::render>("sight::viz::scene3d::render");

            const std::string config =
                "<inout key='offscreen' uid='dummy' />"
                "<scene>"
                "<background color='#000000' />"
                "<layer id='default'>"
                "<viewport hOffset='0.1' vOffset='0.2' width='0.5' height='0.8' hAlign='center' vAlign='bottom' />"
                "</layer>"
                "</scene>";

            render_srv->set_config(config);
            render_srv->set_inout(offscreen, "offscreen");
            render_srv->configure();

            sight::service::remove(render_srv);
        }

        SUBCASE("layer_without_default light")
        {
            auto offscreen  = std::make_shared<sight::data::image>();
            auto render_srv = sight::service::add<sight::viz::scene3d::render>("sight::viz::scene3d::render");

            const std::string config =
                "<inout key='offscreen' uid='dummy' />"
                "<scene>"
                "<background color='#000000' />"
                "<layer id='default' defaultLight='false' />"
                "</scene>";

            render_srv->set_config(config);
            render_srv->set_inout(offscreen, "offscreen");
            render_srv->configure();

            sight::service::remove(render_srv);
        }

        SUBCASE("layer_with_compositors")
        {
            auto offscreen  = std::make_shared<sight::data::image>();
            auto render_srv = sight::service::add<sight::viz::scene3d::render>("sight::viz::scene3d::render");

            const std::string config =
                "<inout key='offscreen' uid='dummy' />"
                "<scene>"
                "<background color='#000000' />"
                "<layer id='default' compositors='Bloom;Motion Blur' />"
                "</scene>";

            render_srv->set_config(config);
            render_srv->set_inout(offscreen, "offscreen");
            render_srv->configure();

            sight::service::remove(render_srv);
        }

        SUBCASE("layer_with_adaptors")
        {
            auto offscreen  = std::make_shared<sight::data::image>();
            auto render_srv = sight::service::add<sight::viz::scene3d::render>("sight::viz::scene3d::render");

            const std::string config =
                "<inout key='offscreen' uid='dummy' />"
                "<scene>"
                "<background color='#000000' />"
                "<layer id='default'>"
                "<adaptor uid='adaptor1' />"
                "<adaptor uid='adaptor2' />"
                "</layer>"
                "</scene>";

            render_srv->set_config(config);
            render_srv->set_inout(offscreen, "offscreen");
            render_srv->configure();

            sight::service::remove(render_srv);
        }
    }

    TEST_CASE("configure_offscreen")
    {
        SUBCASE("offscreen with custom dimensions")
        {
            auto offscreen  = std::make_shared<sight::data::image>();
            auto render_srv = sight::service::add<sight::viz::scene3d::render>("sight::viz::scene3d::render");

            const std::string config =
                "<inout key='offscreen' uid='dummy' />"
                "<scene width='1920' height='1080'>"
                "<background color='#000000' />"
                "<layer id='default' />"
                "</scene>";

            render_srv->set_config(config);
            render_srv->set_inout(offscreen, "offscreen");
            render_srv->configure();

            sight::service::remove(render_srv);
        }

        SUBCASE("offscreen with flip option")
        {
            auto offscreen  = std::make_shared<sight::data::image>();
            auto render_srv = sight::service::add<sight::viz::scene3d::render>("sight::viz::scene3d::render");

            const std::string config =
                "<inout key='offscreen' uid='dummy' />"
                "<scene width='800' height='600' flip='true'>"
                "<background color='#000000' />"
                "<layer id='default' />"
                "</scene>";

            render_srv->set_config(config);
            render_srv->set_inout(offscreen, "offscreen");
            render_srv->configure();

            sight::service::remove(render_srv);
        }
    }

    TEST_CASE("configure_fullscreen")
    {
        auto offscreen  = std::make_shared<sight::data::image>();
        auto render_srv = sight::service::add<sight::viz::scene3d::render>("sight::viz::scene3d::render");

        const std::string config =
            "<inout key='offscreen' uid='dummy' />"
            "<scene fullscreen='true'>"
            "<background color='#000000' />"
            "<layer id='default' />"
            "</scene>";

        render_srv->set_config(config);
        render_srv->set_inout(offscreen, "offscreen");
        render_srv->configure();

        sight::service::remove(render_srv);
    }

    TEST_CASE("configure_complex_scene")
    {
        auto offscreen  = std::make_shared<sight::data::image>();
        auto render_srv = sight::service::add<sight::viz::scene3d::render>("sight::viz::scene3d::render");

        const std::string config =
            "<inout key='offscreen' uid='dummy' />"
            "<scene width='1280' height='720' flip='false' fullscreen='false' render_mode='auto'>"
            "<background top_color='#336699' bottom_color='#003366' top_scale='0.25' bottom_scale='0.75' />"
            "<layer id='main' transparency='DepthPeeling' numPeels='6' defaultLight='true' compositors='HDR'>"
            "<viewport hOffset='0' vOffset='0' width='0.75' height='1.0' hAlign='left' vAlign='top' />"
            "<adaptor uid='mainAdaptor1' />"
            "<adaptor uid='mainAdaptor2' />"
            "</layer>"
            "<layer id='overlay' defaultLight='false'>"
            "<viewport hOffset='0' vOffset='0' width='0.25' height='0.25' hAlign='right' vAlign='top' />"
            "<adaptor uid='overlayAdaptor' />"
            "</layer>"
            "</scene>";

        render_srv->set_config(config);
        render_srv->set_inout(offscreen, "offscreen");
        render_srv->configure();

        sight::service::remove(render_srv);
    }
}
