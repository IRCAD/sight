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

#include "test_services.hpp"

#include <core/runtime/path.hpp>
#include <core/runtime/runtime.hpp>

#include <data/image.hpp>
#include <data/mesh.hpp>
#include <data/transfer_function.hpp>

#include <service/op.hpp>

#include <app/config_manager.hpp>
#include <app/parser/image.hpp>
#include <app/parser/transfer_function.hpp>

#include <boost/property_tree/xml_parser.hpp>

#include <doctest/doctest.h>

#include <glm/common.hpp>
#include <glm/gtc/epsilon.hpp>

static const double EPSILON = 1e-5;

// There might be some uncertainty when sampling, so we need to include an epsilon when testing equality
#define ASSERT_COLOR_EQUALS(c1, c2) \
        CHECK(glm::all(glm::epsilonEqual(c1, c2, EPSILON)));

//------------------------------------------------------------------------------

TEST_SUITE("sight::app::data_parser")
{
    struct data_parser_fixture
    {
        data_parser_fixture()
        {
            // Set up context before running a test
            sight::core::runtime::init();

            std::filesystem::path location = sight::core::runtime::get_resource_file_path("app_ut");
            CHECK(std::filesystem::exists(location));

            sight::core::runtime::add_modules(location);
            sight::core::runtime::load_module("sight::module::app");
        }

        //------------------------------------------------------------------------------

        static sight::service::config_t build_object_config()
        {
            sight::service::config_t config;

            // Configuration on core::object which uid is objectUUID
            sight::service::config_t obj_cfg;
            obj_cfg.add("<xmlattr>.uid", "objectUUID");
            obj_cfg.add("<xmlattr>.type", "sight::data::image");
            config.add_child("object", obj_cfg);

            // Object's service A
            sight::service::config_t service_a;
            service_a.add("<xmlattr>.uid", "myTestService1");
            service_a.add("<xmlattr>.type", "sight::app::ut::test1_image");

            sight::service::config_t data_service_a;
            data_service_a.add("<xmlattr>.key", "data");
            data_service_a.add("<xmlattr>.uid", "objectUUID");
            service_a.add_child("in", data_service_a);
            config.add_child("service", service_a);

            // Object's service B
            sight::service::config_t service_b;
            service_b.add("<xmlattr>.uid", "myTestService2");
            service_b.add("<xmlattr>.type", "sight::app::ut::test1_image");
            config.add_child("service", service_b);

            // Update method from object's services
            sight::service::config_t update1;
            update1.add("<xmlattr>.uid", "myTestService1");
            config.add_child("update", update1);

            return config;
        }
    };

//------------------------------------------------------------------------------

    TEST_CASE_FIXTURE(data_parser_fixture, "object_creation_with_config")
    {
        const std::string object_uuid    = "objectUUID";
        const std::string service_uui_d1 = "myTestService1";
        const std::string service_uui_d2 = "myTestService2";

        // Create object configuration
        const auto config = build_object_config();

        // Create the object and its services from the configuration
        auto config_manager = sight::app::config_manager::make();
        config_manager->sight::app::config_manager::set_config(config);
        config_manager->create();
        auto image = std::dynamic_pointer_cast<sight::data::image>(config_manager->get_config_root());

        // Test object uid
        CHECK_EQ(object_uuid, image->get_id());

        // Test start services
        config_manager->start();
        const auto& srv1 = sight::service::get(service_uui_d1);
        const auto& srv2 = sight::service::get(service_uui_d2);
        CHECK(srv1->started());
        CHECK(srv2->started());

        // Test if object's service is created
        CHECK(image == srv1->sight::data::has_data::object("data", sight::data::access::in));

        // Test update services
        config_manager->update();
        CHECK(std::dynamic_pointer_cast<sight::app::ut::test_config_service>(srv1)->is_updated());
        CHECK(std::dynamic_pointer_cast<sight::app::ut::test_config_service>(srv2)->is_updated() == false);

        // Test stop services
        config_manager->stop();
        CHECK(srv1->stopped());
        CHECK(srv2->stopped());

        config_manager->destroy();
    }

//------------------------------------------------------------------------------

    TEST_CASE_FIXTURE(data_parser_fixture, "image_parser")
    {
        const std::string object_uuid = "objectUUID";
        sight::service::config_t config;

        // Configuration on core::object which uid is objectUUID
        sight::service::config_t obj_cfg;
        obj_cfg.add("<xmlattr>.uid", object_uuid);
        obj_cfg.add("<xmlattr>.type", "sight::data::image");
        obj_cfg.add("color", "#FF459812");
        config.add_child("object", obj_cfg);

        // Create the object and its services from the configuration
        auto config_manager = sight::app::config_manager::make();
        config_manager->sight::app::config_manager::set_config(config);
        config_manager->create();
        auto image = std::dynamic_pointer_cast<sight::data::image>(sight::core::id::get_object(object_uuid));

        // Test object uid
        CHECK_EQ(object_uuid, image->get_id());
        CHECK_EQ(sight::data::image::rgba, image->pixel_format());
        CHECK_EQ(sight::core::type::UINT8, image->type());

        // We only test the image content, we do not really care about the image size and other attributes for now
        const auto dump_lock = image->dump_lock();
        auto itr             = image->begin<sight::data::iterator::rgba>();
        const auto itr_end   = image->end<sight::data::iterator::rgba>();

        for( ; itr != itr_end ; ++itr)
        {
            CHECK_EQ(std::uint8_t(0xFF), itr->r);
            CHECK_EQ(std::uint8_t(0x45), itr->g);
            CHECK_EQ(std::uint8_t(0x98), itr->b);
            CHECK_EQ(std::uint8_t(0X12), itr->a);
        }

        config_manager->destroy();
    }

//------------------------------------------------------------------------------

    TEST_CASE_FIXTURE(data_parser_fixture, "transfer_function_parser")
    {
        {
            sight::service::config_t config;

            std::stringstream config_string;
            config_string << "<colors default=\"true\" />";
            boost::property_tree::read_xml(config_string, config);

            auto parser =
                sight::service::add<sight::app::parser::transfer_function>("sight::app::parser::transfer_function");
            CHECK(parser->is_a("sight::app::parser::transfer_function"));

            auto tf = std::make_shared<sight::data::transfer_function>();
            sight::service::object_parser::objects_t sub_objects;
            parser->parse(config, tf, sub_objects);

            CHECK_MESSAGE(std::abs(50.0 - tf->level()) < EPSILON, "Wrong level ", tf->level());
            CHECK_MESSAGE(std::abs(500.0 - tf->window()) < EPSILON, "Wrong window", tf->window());

            CHECK_MESSAGE(
                std::abs(50.0 - tf->pieces()[0]->level()) < EPSILON,
                "Wrong level ",
                tf->pieces()[0]->level()
            );
            CHECK_MESSAGE(
                std::abs(500.0 - tf->pieces()[0]->window()) < EPSILON,
                "Wrong window",
                tf->pieces()[0]->window()
            );

            CHECK_EQ(sight::data::transfer_function::DEFAULT_TF_NAME, tf->name());
            CHECK(sight::data::transfer_function::color_t() == tf->background_color());

            const auto first_piece = tf->pieces().front();

            CHECK_EQ(
                sight::data::transfer_function::interpolation_mode::linear,
                first_piece->get_interpolation_mode()
            );
            CHECK_EQ(false, first_piece->clamped());
            CHECK_EQ(std::size_t(2), first_piece->size());
        }

        {
            sight::service::config_t config;

            std::string name = "test_tf";

            std::stringstream config_string;
            config_string
            << "<name>" + name + "</name>"
                                 "<colors>"
                                 "<step color=\"#ffff00ff\" value=\"-200\" />"
                                 "<step color=\"#000000ff\" value=\"0\" />"
                                 "<step color=\"#0000ffff\" value=\"1\" />"
                                 "<step color=\"#0000ffff\" value=\"500\" />"
                                 "<step color=\"#00ff00ff\" value=\"1000\" />"
                                 "<step color=\"#ff0000ff\" value=\"1500\" />"
                                 "<step color=\"#000000ff\" value=\"5000\" />"
                                 "</colors>";
            boost::property_tree::read_xml(config_string, config);

            auto parser =
                sight::service::add<sight::app::parser::transfer_function>("sight::app::parser::transfer_function");
            CHECK(parser->is_a("sight::app::parser::transfer_function"));

            auto tf = std::make_shared<sight::data::transfer_function>();

            sight::service::object_parser::objects_t sub_objects;
            parser->parse(config, tf, sub_objects);

            CHECK_EQ(tf->name(), name);

            const auto piece = tf->pieces().front();
            CHECK_EQ(std::size_t(7), piece->size());

            CHECK_EQ(-200., piece->min_max().first);
            CHECK_EQ(5000., piece->min_max().second);
            CHECK_EQ(5200., piece->window());
            CHECK_EQ(2400., piece->level());
            ASSERT_COLOR_EQUALS(sight::data::transfer_function::color_t(1., 1., 0., 1.), piece->sample_linear(-200));
            ASSERT_COLOR_EQUALS(sight::data::transfer_function::color_t(0., 0., 0., 1.), piece->sample_linear(0));
            ASSERT_COLOR_EQUALS(sight::data::transfer_function::color_t(0., 0., 1., 1.), piece->sample_linear(250));
            ASSERT_COLOR_EQUALS(sight::data::transfer_function::color_t(0., 0., 1., 1.), piece->sample_linear(500));
            ASSERT_COLOR_EQUALS(sight::data::transfer_function::color_t(0., 1., 0., 1.), piece->sample_linear(1000));
            ASSERT_COLOR_EQUALS(sight::data::transfer_function::color_t(1., 0., 0., 1.), piece->sample_linear(1500));
            ASSERT_COLOR_EQUALS(sight::data::transfer_function::color_t(0., 0., 0., 1.), piece->sample_linear(5000));

            CHECK_EQ(-200., tf->min_max().first);
            CHECK_EQ(5000., tf->min_max().second);
            CHECK_EQ(5200., tf->window());
            CHECK_EQ(2400., tf->level());
            ASSERT_COLOR_EQUALS(sight::data::transfer_function::color_t(1., 1., 0., 1.), tf->sample_linear(-200));
            ASSERT_COLOR_EQUALS(sight::data::transfer_function::color_t(0., 0., 0., 1.), tf->sample_linear(0));
            ASSERT_COLOR_EQUALS(sight::data::transfer_function::color_t(0., 0., 1., 1.), tf->sample_linear(250));
            ASSERT_COLOR_EQUALS(sight::data::transfer_function::color_t(0., 0., 1., 1.), tf->sample_linear(500));
            ASSERT_COLOR_EQUALS(sight::data::transfer_function::color_t(0., 1., 0., 1.), tf->sample_linear(1000));
            ASSERT_COLOR_EQUALS(sight::data::transfer_function::color_t(1., 0., 0., 1.), tf->sample_linear(1500));
            ASSERT_COLOR_EQUALS(sight::data::transfer_function::color_t(0., 0., 0., 1.), tf->sample_linear(5000));
        }
    }

//------------------------------------------------------------------------------
} // TEST_SUITE
