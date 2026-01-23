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

#include <boost/property_tree/xml_parser.hpp>

#include <doctest/doctest.h>

TEST_SUITE("sight::viz::scene3d::render")
{
    TEST_CASE("configure_render_mode")
    {
        SUBCASE("auto mode")
        {
            auto offscreen  = std::make_shared<sight::data::image>();
            auto render_srv = sight::service::add<sight::viz::scene3d::render>("sight::viz::scene3d::render");

            sight::service::config_t config;
            std::stringstream config_string;
            config_string << R"(<inout key="offscreen" uid="dummy" />)"
            << R"(<scene render_mode="auto">)"
               R"( <background color="#36393E" />)"
               R"( <layer id="default" />)"
               R"(</scene>)";

            boost::property_tree::read_xml(config_string, config);
            render_srv->set_config(config);
            render_srv->set_inout(offscreen, "offscreen");
            render_srv->configure();

            CHECK(render_srv->get_render_mode() == sight::viz::scene3d::render::render_mode::automatic);

            sight::service::remove(render_srv);
        }

        SUBCASE("manual mode")
        {
            auto offscreen  = std::make_shared<sight::data::image>();
            auto render_srv = sight::service::add<sight::viz::scene3d::render>("sight::viz::scene3d::render");

            sight::service::config_t config;
            std::stringstream config_string;
            config_string << R"(<inout key="offscreen" uid="dummy" />)"
            << R"(<scene render_mode="manual">)"
               R"( <background color="#36393E" />)"
               R"( <layer id="default" />)"
               R"(</scene>)";

            boost::property_tree::read_xml(config_string, config);
            render_srv->set_config(config);
            render_srv->set_inout(offscreen, "offscreen");
            render_srv->configure();

            CHECK(render_srv->get_render_mode() == sight::viz::scene3d::render::render_mode::manual);

            sight::service::remove(render_srv);
        }

        SUBCASE("default mode (no render_mode attribute)")
        {
            auto offscreen  = std::make_shared<sight::data::image>();
            auto render_srv = sight::service::add<sight::viz::scene3d::render>("sight::viz::scene3d::render");

            sight::service::config_t config;
            std::stringstream config_string;
            config_string << R"(<inout key="offscreen" uid="dummy" />)"
            << R"(<scene>)"
               R"( <background color="#36393E" />)"
               R"( <layer id="default" />)"
               R"(</scene>)";

            boost::property_tree::read_xml(config_string, config);
            render_srv->set_config(config);
            render_srv->set_inout(offscreen, "offscreen");
            render_srv->configure();

            CHECK(render_srv->get_render_mode() == sight::viz::scene3d::render::render_mode::automatic);

            sight::service::remove(render_srv);
        }
    }

    TEST_CASE("configure_background")
    {
        SUBCASE("background with single color")
        {
            auto offscreen  = std::make_shared<sight::data::image>();
            auto render_srv = sight::service::add<sight::viz::scene3d::render>("sight::viz::scene3d::render");

            sight::service::config_t config;
            std::stringstream config_string;
            config_string << R"(<inout key="offscreen" uid="dummy" />)"
            << R"(<scene>)"
               R"( <background color="#FF0000" />)"
               R"( <layer id="default" />)"
               R"(</scene>)";

            boost::property_tree::read_xml(config_string, config);
            render_srv->set_config(config);
            render_srv->set_inout(offscreen, "offscreen");
            render_srv->configure();

            sight::service::remove(render_srv);
        }

        SUBCASE("background with gradient colors")
        {
            auto offscreen  = std::make_shared<sight::data::image>();
            auto render_srv = sight::service::add<sight::viz::scene3d::render>("sight::viz::scene3d::render");

            sight::service::config_t config;
            std::stringstream config_string;
            config_string << R"(<inout key="offscreen" uid="dummy" />)"
            << R"(<scene>)"
               R"( <background top_color="#FF0000" bottom_color="#0000FF" />)"
               R"( <layer id="default" />)"
               R"(</scene>)";

            boost::property_tree::read_xml(config_string, config);
            render_srv->set_config(config);
            render_srv->set_inout(offscreen, "offscreen");
            render_srv->configure();

            sight::service::remove(render_srv);
        }

        SUBCASE("background with scales")
        {
            auto offscreen  = std::make_shared<sight::data::image>();
            auto render_srv = sight::service::add<sight::viz::scene3d::render>("sight::viz::scene3d::render");

            sight::service::config_t config;
            std::stringstream config_string;
            config_string << R"(<inout key="offscreen" uid="dummy" />)"
            << R"(<scene>)"
               R"( <background top_color="#FF0000" bottom_color="#0000FF" top_scale="0.3" bottom_scale="0.7" />)"
               R"( <layer id="default" />)"
               R"(</scene>)";

            boost::property_tree::read_xml(config_string, config);
            render_srv->set_config(config);
            render_srv->set_inout(offscreen, "offscreen");
            render_srv->configure();

            sight::service::remove(render_srv);
        }

        SUBCASE("background with material")
        {
            auto offscreen  = std::make_shared<sight::data::image>();
            auto render_srv = sight::service::add<sight::viz::scene3d::render>("sight::viz::scene3d::render");

            sight::service::config_t config;
            std::stringstream config_string;
            config_string << R"(<inout key="offscreen" uid="dummy" />)"
            << R"(<scene>)"
               R"( <background material="CustomMaterial" />)"
               R"( <layer id="default" />)"
               R"(</scene>)";

            boost::property_tree::read_xml(config_string, config);
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

            sight::service::config_t config;
            std::stringstream config_string;
            config_string << R"(<inout key="offscreen" uid="dummy" />)"
            << R"(<scene>)"
               R"( <background color="#000000" />)"
               R"( <layer id="default" />)"
               R"(</scene>)";

            boost::property_tree::read_xml(config_string, config);
            render_srv->set_config(config);
            render_srv->set_inout(offscreen, "offscreen");
            render_srv->configure();

            sight::service::remove(render_srv);
        }

        SUBCASE("multiple layers")
        {
            auto offscreen  = std::make_shared<sight::data::image>();
            auto render_srv = sight::service::add<sight::viz::scene3d::render>("sight::viz::scene3d::render");

            sight::service::config_t config;
            std::stringstream config_string;
            config_string << R"(<inout key="offscreen" uid="dummy" />)"
            << R"(<scene>)"
               R"( <background color="#000000" />)"
               R"( <layer id="layer1" />)"
               R"( <layer id="layer2" />)"
               R"( <layer id="layer3" />)"
               R"(</scene>)";

            boost::property_tree::read_xml(config_string, config);
            render_srv->set_config(config);
            render_srv->set_inout(offscreen, "offscreen");
            render_srv->configure();

            sight::service::remove(render_srv);
        }

        SUBCASE("layer with transparency")
        {
            auto offscreen  = std::make_shared<sight::data::image>();
            auto render_srv = sight::service::add<sight::viz::scene3d::render>("sight::viz::scene3d::render");

            sight::service::config_t config;
            std::stringstream config_string;
            config_string << R"(<inout key="offscreen" uid="dummy" />)"
            << R"(<scene>)"
               R"( <background color="#000000" />)"
               R"( <layer id="default" transparency="DepthPeeling" numPeels="8" />)"
               R"(</scene>)";

            boost::property_tree::read_xml(config_string, config);
            render_srv->set_config(config);
            render_srv->set_inout(offscreen, "offscreen");
            render_srv->configure();

            sight::service::remove(render_srv);
        }

        SUBCASE("layer with stereo mode")
        {
            auto offscreen  = std::make_shared<sight::data::image>();
            auto render_srv = sight::service::add<sight::viz::scene3d::render>("sight::viz::scene3d::render");

            sight::service::config_t config;
            std::stringstream config_string;
            config_string << R"(<inout key="offscreen" uid="dummy" />)"
            << R"(<scene>)"
               R"( <background color="#000000" />)"
               R"( <layer id="default" stereoMode="AutoStereo5" />)"
               R"(</scene>)";

            boost::property_tree::read_xml(config_string, config);
            render_srv->set_config(config);
            render_srv->set_inout(offscreen, "offscreen");
            render_srv->configure();

            sight::service::remove(render_srv);
        }

        SUBCASE("layer with viewport configuration")
        {
            auto offscreen  = std::make_shared<sight::data::image>();
            auto render_srv = sight::service::add<sight::viz::scene3d::render>("sight::viz::scene3d::render");

            sight::service::config_t config;
            std::stringstream config_string;
            config_string << R"(<inout key="offscreen" uid="dummy" />)"
            << R"(<scene>)"
               R"( <background color="#000000" />)"
               R"( <layer id="default">)"
               R"(  <viewport hOffset="0.1" vOffset="0.2" width="0.5" height="0.8" hAlign="center" vAlign="bottom" />)"
               R"( </layer>)"
               R"(</scene>)";

            boost::property_tree::read_xml(config_string, config);
            render_srv->set_config(config);
            render_srv->set_inout(offscreen, "offscreen");
            render_srv->configure();

            sight::service::remove(render_srv);
        }

        SUBCASE("layer without default light")
        {
            auto offscreen  = std::make_shared<sight::data::image>();
            auto render_srv = sight::service::add<sight::viz::scene3d::render>("sight::viz::scene3d::render");

            sight::service::config_t config;
            std::stringstream config_string;
            config_string << R"(<inout key="offscreen" uid="dummy" />)"
            << R"(<scene>)"
               R"( <background color="#000000" />)"
               R"( <layer id="default" defaultLight="false" />)"
               R"(</scene>)";

            boost::property_tree::read_xml(config_string, config);
            render_srv->set_config(config);
            render_srv->set_inout(offscreen, "offscreen");
            render_srv->configure();

            sight::service::remove(render_srv);
        }

        SUBCASE("layer with compositors")
        {
            auto offscreen  = std::make_shared<sight::data::image>();
            auto render_srv = sight::service::add<sight::viz::scene3d::render>("sight::viz::scene3d::render");

            sight::service::config_t config;
            std::stringstream config_string;
            config_string << R"(<inout key="offscreen" uid="dummy" />)"
            << R"(<scene>)"
               R"( <background color="#000000" />)"
               R"( <layer id="default" compositors="Bloom;Motion Blur" />)"
               R"(</scene>)";

            boost::property_tree::read_xml(config_string, config);
            render_srv->set_config(config);
            render_srv->set_inout(offscreen, "offscreen");
            render_srv->configure();

            sight::service::remove(render_srv);
        }

        SUBCASE("layer with adaptors")
        {
            auto offscreen  = std::make_shared<sight::data::image>();
            auto render_srv = sight::service::add<sight::viz::scene3d::render>("sight::viz::scene3d::render");

            sight::service::config_t config;
            std::stringstream config_string;
            config_string << R"(<inout key="offscreen" uid="dummy" />)"
            << R"(<scene>)"
               R"( <background color="#000000" />)"
               R"( <layer id="default">)"
               R"(  <adaptor uid="adaptor1" />)"
               R"(  <adaptor uid="adaptor2" />)"
               R"( </layer>)"
               R"(</scene>)";

            boost::property_tree::read_xml(config_string, config);
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

            sight::service::config_t config;
            std::stringstream config_string;
            config_string << R"(<inout key="offscreen" uid="dummy" />)"
            << R"(<scene width="1920" height="1080">)"
               R"( <background color="#000000" />)"
               R"( <layer id="default" />)"
               R"(</scene>)";

            boost::property_tree::read_xml(config_string, config);
            render_srv->set_config(config);
            render_srv->set_inout(offscreen, "offscreen");
            render_srv->configure();

            sight::service::remove(render_srv);
        }

        SUBCASE("offscreen with flip option")
        {
            auto offscreen  = std::make_shared<sight::data::image>();
            auto render_srv = sight::service::add<sight::viz::scene3d::render>("sight::viz::scene3d::render");

            sight::service::config_t config;
            std::stringstream config_string;
            config_string << R"(<inout key="offscreen" uid="dummy" />)"
            << R"(<scene width="800" height="600" flip="true">)"
               R"( <background color="#000000" />)"
               R"( <layer id="default" />)"
               R"(</scene>)";

            boost::property_tree::read_xml(config_string, config);
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

        sight::service::config_t config;
        std::stringstream config_string;
        config_string << R"(<inout key="offscreen" uid="dummy" />)"
        << R"(<scene fullscreen="true">)"
           R"( <background color="#000000" />)"
           R"( <layer id="default" />)"
           R"(</scene>)";

        boost::property_tree::read_xml(config_string, config);
        render_srv->set_config(config);
        render_srv->set_inout(offscreen, "offscreen");
        render_srv->configure();

        sight::service::remove(render_srv);
    }

    TEST_CASE("configure_complex_scene")
    {
        auto offscreen  = std::make_shared<sight::data::image>();
        auto render_srv = sight::service::add<sight::viz::scene3d::render>("sight::viz::scene3d::render");

        sight::service::config_t config;
        std::stringstream config_string;
        config_string << R"(<inout key="offscreen" uid="dummy" />)"
        << R"(<scene width="1280" height="720" flip="false" fullscreen="false" render_mode="auto">)"
           R"( <background top_color="#336699" bottom_color="#003366" top_scale="0.25" bottom_scale="0.75" />)"
           R"( <layer id="main" transparency="DepthPeeling" numPeels="6" defaultLight="true" compositors="HDR">)"
           R"(  <viewport hOffset="0" vOffset="0" width="0.75" height="1.0" hAlign="left" vAlign="top" />)"
           R"(  <adaptor uid="mainAdaptor1" />)"
           R"(  <adaptor uid="mainAdaptor2" />)"
           R"( </layer>)"
           R"( <layer id="overlay" defaultLight="false">)"
           R"(  <viewport hOffset="0" vOffset="0" width="0.25" height="0.25" hAlign="right" vAlign="top" />)"
           R"(  <adaptor uid="overlayAdaptor" />)"
           R"( </layer>)"
           R"(</scene>)";

        boost::property_tree::read_xml(config_string, config);
        render_srv->set_config(config);
        render_srv->set_inout(offscreen, "offscreen");
        render_srv->configure();

        sight::service::remove(render_srv);
    }
}
