/************************************************************************
 *
 * Copyright (C) 2009-2023 IRCAD France
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

#include "config_parser_test.hpp"

#include "test_services.hpp"

#include <app/config_manager.hpp>
#include <app/parser/image.hpp>
#include <app/parser/transfer_function.hpp>

#include <core/runtime/path.hpp>
#include <core/runtime/runtime.hpp>

#include <data/image.hpp>
#include <data/mesh.hpp>
#include <data/transfer_function.hpp>

#include <service/op.hpp>

#include <boost/property_tree/xml_parser.hpp>

#include <glm/common.hpp>
#include <glm/gtc/epsilon.hpp>

static const double EPSILON = 1e-5;

// There might be some uncertainty when sampling, so we need to include an epsilon when testing equality
#define ASSERT_COLOR_EQUALS(c1, c2) \
    CPPUNIT_ASSERT(glm::all(glm::epsilonEqual(c1, c2, EPSILON)));

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION(sight::app::ut::data_parser_test);

//------------------------------------------------------------------------------

namespace sight::app::ut
{

//------------------------------------------------------------------------------

void data_parser_test::setUp()
{
    // Set up context before running a test
    core::runtime::init();

    std::filesystem::path location = core::runtime::get_resource_file_path("tu_exec_app");
    CPPUNIT_ASSERT(std::filesystem::exists(location));

    core::runtime::add_modules(location);
    core::runtime::load_module("sight::module::app");
}

//------------------------------------------------------------------------------

void data_parser_test::tearDown()
{
    // Clean up after the test run.
}

//------------------------------------------------------------------------------

void data_parser_test::test_object_creation_with_config()
{
    const std::string object_uuid    = "objectUUID";
    const std::string service_uui_d1 = "myTestService1";
    const std::string service_uui_d2 = "myTestService2";

    // Create object configuration
    const auto config = build_object_config();

    // Create the object and its services from the configuration
    auto config_manager = app::config_manager::make();
    config_manager->app::config_manager::set_config(config);
    config_manager->create();
    auto image = std::dynamic_pointer_cast<data::image>(config_manager->get_config_root());

    // Test object uid
    CPPUNIT_ASSERT_EQUAL(object_uuid, image->get_id());

    // Test start services
    config_manager->start();
    const auto& srv1 = service::get(service_uui_d1);
    const auto& srv2 = service::get(service_uui_d2);
    CPPUNIT_ASSERT(srv1->started());
    CPPUNIT_ASSERT(srv2->started());

    // Test if object's service is created
    CPPUNIT_ASSERT(image == srv1->data::has_data::object("data", data::access::in));

    // Test update services
    config_manager->update();
    CPPUNIT_ASSERT(std::dynamic_pointer_cast<app::ut::test_config_service>(srv1)->get_is_updated());
    CPPUNIT_ASSERT(std::dynamic_pointer_cast<app::ut::test_config_service>(srv2)->get_is_updated() == false);

    // Test stop services
    config_manager->stop();
    CPPUNIT_ASSERT(srv1->stopped());
    CPPUNIT_ASSERT(srv2->stopped());

    config_manager->destroy();
}

//------------------------------------------------------------------------------

void data_parser_test::test_image_parser()
{
    const std::string object_uuid = "objectUUID";
    service::config_t config;

    // Configuration on core::tools::object which uid is objectUUID
    service::config_t obj_cfg;
    obj_cfg.add("<xmlattr>.uid", object_uuid);
    obj_cfg.add("<xmlattr>.type", "sight::data::image");
    obj_cfg.add("color", "#FF459812");
    config.add_child("object", obj_cfg);

    // Create the object and its services from the configuration
    auto config_manager = app::config_manager::make();
    config_manager->app::config_manager::set_config(config);
    config_manager->create();
    auto image = std::dynamic_pointer_cast<data::image>(core::tools::id::get_object(object_uuid));

    // Test object uid
    CPPUNIT_ASSERT_EQUAL(object_uuid, image->get_id());
    CPPUNIT_ASSERT_EQUAL(sight::data::image::rgba, image->pixel_format());
    CPPUNIT_ASSERT_EQUAL(sight::core::type::UINT8, image->type());

    // We only test the image content, we do not really care about the image size and other attributes for now
    const auto dump_lock = image->dump_lock();
    auto itr             = image->begin<sight::data::iterator::rgba>();
    const auto itr_end   = image->end<sight::data::iterator::rgba>();

    for( ; itr != itr_end ; ++itr)
    {
        CPPUNIT_ASSERT_EQUAL(std::uint8_t(0xFF), itr->r);
        CPPUNIT_ASSERT_EQUAL(std::uint8_t(0x45), itr->g);
        CPPUNIT_ASSERT_EQUAL(std::uint8_t(0x98), itr->b);
        CPPUNIT_ASSERT_EQUAL(std::uint8_t(0X12), itr->a);
    }

    config_manager->destroy();
}

//------------------------------------------------------------------------------

void data_parser_test::test_transfer_function_parser()
{
    service::config_t config;

    std::stringstream config_string;
    config_string << "<colors>"
                     "<step color=\"#ffff00ff\" value=\"-200\" />"
                     "<step color=\"#000000ff\" value=\"0\" />"
                     "<step color=\"#0000ffff\" value=\"1\" />"
                     "<step color=\"#0000ffff\" value=\"500\" />"
                     "<step color=\"#00ff00ff\" value=\"1000\" />"
                     "<step color=\"#ff0000ff\" value=\"1500\" />"
                     "<step color=\"#000000ff\" value=\"5000\" />"
                     "</colors>";
    boost::property_tree::read_xml(config_string, config);

    auto parser = sight::service::add<sight::app::parser::transfer_function>("sight::app::parser::transfer_function");
    parser->set_object_config(config);

    auto tf = std::make_shared<sight::data::transfer_function>();
    parser->create_config(tf);

    const auto piece = tf->pieces().front();
    CPPUNIT_ASSERT_EQUAL(std::size_t(7), piece->size());

    CPPUNIT_ASSERT_EQUAL(-200., piece->min_max().first);
    CPPUNIT_ASSERT_EQUAL(5000., piece->min_max().second);
    CPPUNIT_ASSERT_EQUAL(5200., piece->window());
    CPPUNIT_ASSERT_EQUAL(2400., piece->level());
    ASSERT_COLOR_EQUALS(data::transfer_function::color_t(1., 1., 0., 1.), piece->sample_linear(-200));
    ASSERT_COLOR_EQUALS(data::transfer_function::color_t(0., 0., 0., 1.), piece->sample_linear(0));
    ASSERT_COLOR_EQUALS(data::transfer_function::color_t(0., 0., 1., 1.), piece->sample_linear(250));
    ASSERT_COLOR_EQUALS(data::transfer_function::color_t(0., 0., 1., 1.), piece->sample_linear(500));
    ASSERT_COLOR_EQUALS(data::transfer_function::color_t(0., 1., 0., 1.), piece->sample_linear(1000));
    ASSERT_COLOR_EQUALS(data::transfer_function::color_t(1., 0., 0., 1.), piece->sample_linear(1500));
    ASSERT_COLOR_EQUALS(data::transfer_function::color_t(0., 0., 0., 1.), piece->sample_linear(5000));

    CPPUNIT_ASSERT_EQUAL(-200., tf->min_max().first);
    CPPUNIT_ASSERT_EQUAL(5000., tf->min_max().second);
    CPPUNIT_ASSERT_EQUAL(5200., tf->window());
    CPPUNIT_ASSERT_EQUAL(2400., tf->level());
    ASSERT_COLOR_EQUALS(data::transfer_function::color_t(1., 1., 0., 1.), tf->sample_linear(-200));
    ASSERT_COLOR_EQUALS(data::transfer_function::color_t(0., 0., 0., 1.), tf->sample_linear(0));
    ASSERT_COLOR_EQUALS(data::transfer_function::color_t(0., 0., 1., 1.), tf->sample_linear(250));
    ASSERT_COLOR_EQUALS(data::transfer_function::color_t(0., 0., 1., 1.), tf->sample_linear(500));
    ASSERT_COLOR_EQUALS(data::transfer_function::color_t(0., 1., 0., 1.), tf->sample_linear(1000));
    ASSERT_COLOR_EQUALS(data::transfer_function::color_t(1., 0., 0., 1.), tf->sample_linear(1500));
    ASSERT_COLOR_EQUALS(data::transfer_function::color_t(0., 0., 0., 1.), tf->sample_linear(5000));
}

//------------------------------------------------------------------------------

service::config_t data_parser_test::build_object_config()
{
    service::config_t config;

    // Configuration on core::tools::object which uid is objectUUID
    service::config_t obj_cfg;
    obj_cfg.add("<xmlattr>.uid", "objectUUID");
    obj_cfg.add("<xmlattr>.type", "sight::data::image");
    config.add_child("object", obj_cfg);

    // Object's service A
    service::config_t service_a;
    service_a.add("<xmlattr>.uid", "myTestService1");
    service_a.add("<xmlattr>.type", "sight::app::ut::test1_image");

    service::config_t data_service_a;
    data_service_a.add("<xmlattr>.key", "data");
    data_service_a.add("<xmlattr>.uid", "objectUUID");
    service_a.add_child("in", data_service_a);
    config.add_child("service", service_a);

    // Object's service B
    service::config_t service_b;
    service_b.add("<xmlattr>.uid", "myTestService2");
    service_b.add("<xmlattr>.type", "sight::app::ut::test1_image");
    config.add_child("service", service_b);

    // Start method from object's services
    service::config_t start_a;
    start_a.add("<xmlattr>.uid", "myTestService1");
    config.add_child("start", start_a);
    service::config_t start_b;
    start_b.add("<xmlattr>.uid", "myTestService2");
    config.add_child("start", start_b);

    // Update method from object's services
    service::config_t update1;
    update1.add("<xmlattr>.uid", "myTestService1");
    config.add_child("update", update1);

    return config;
}

//------------------------------------------------------------------------------

} // namespace sight::app::ut
