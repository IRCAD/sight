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

static const double s_EPSILON = 1e-5;

// There might be some uncertainty when sampling, so we need to include an epsilon when testing equality
#define ASSERT_COLOR_EQUALS(c1, c2) \
    CPPUNIT_ASSERT(glm::all(glm::epsilonEqual(c1, c2, s_EPSILON)));

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

void data_parser_test::testObjectCreationWithConfig()
{
    const std::string objectUUID   = "objectUUID";
    const std::string serviceUUID1 = "myTestService1";
    const std::string serviceUUID2 = "myTestService2";

    // Create object configuration
    const auto config = buildObjectConfig();

    // Create the object and its services from the configuration
    auto configManager = app::config_manager::make();
    configManager->app::config_manager::setConfig(config);
    configManager->create();
    auto image = std::dynamic_pointer_cast<data::image>(configManager->getConfigRoot());

    // Test object uid
    CPPUNIT_ASSERT_EQUAL(objectUUID, image->get_id());

    // Test start services
    configManager->start();
    const auto& srv1 = service::get(serviceUUID1);
    const auto& srv2 = service::get(serviceUUID2);
    CPPUNIT_ASSERT(srv1->started());
    CPPUNIT_ASSERT(srv2->started());

    // Test if object's service is created
    CPPUNIT_ASSERT(image == srv1->data::has_data::object("data", data::Access::in));

    // Test update services
    configManager->update();
    CPPUNIT_ASSERT(std::dynamic_pointer_cast<app::ut::TestConfigService>(srv1)->getIsUpdated());
    CPPUNIT_ASSERT(std::dynamic_pointer_cast<app::ut::TestConfigService>(srv2)->getIsUpdated() == false);

    // Test stop services
    configManager->stop();
    CPPUNIT_ASSERT(srv1->stopped());
    CPPUNIT_ASSERT(srv2->stopped());

    configManager->destroy();
}

//------------------------------------------------------------------------------

void data_parser_test::testImageParser()
{
    const std::string objectUUID = "objectUUID";
    service::config_t config;

    // Configuration on core::tools::object which uid is objectUUID
    service::config_t objCfg;
    objCfg.add("<xmlattr>.uid", objectUUID);
    objCfg.add("<xmlattr>.type", "sight::data::image");
    objCfg.add("color", "#FF459812");
    config.add_child("object", objCfg);

    // Create the object and its services from the configuration
    auto configManager = app::config_manager::make();
    configManager->app::config_manager::setConfig(config);
    configManager->create();
    auto image = std::dynamic_pointer_cast<data::image>(core::tools::id::get_object(objectUUID));

    // Test object uid
    CPPUNIT_ASSERT_EQUAL(objectUUID, image->get_id());
    CPPUNIT_ASSERT_EQUAL(sight::data::image::RGBA, image->getPixelFormat());
    CPPUNIT_ASSERT_EQUAL(sight::core::type::UINT8, image->getType());

    // We only test the image content, we do not really care about the image size and other attributes for now
    const auto dumpLock = image->dump_lock();
    auto itr            = image->begin<sight::data::iterator::rgba>();
    const auto itrEnd   = image->end<sight::data::iterator::rgba>();

    for( ; itr != itrEnd ; ++itr)
    {
        CPPUNIT_ASSERT_EQUAL(std::uint8_t(0xFF), itr->r);
        CPPUNIT_ASSERT_EQUAL(std::uint8_t(0x45), itr->g);
        CPPUNIT_ASSERT_EQUAL(std::uint8_t(0x98), itr->b);
        CPPUNIT_ASSERT_EQUAL(std::uint8_t(0X12), itr->a);
    }

    configManager->destroy();
}

//------------------------------------------------------------------------------

void data_parser_test::testTransferFunctionParser()
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
    parser->setObjectConfig(config);

    auto tf = std::make_shared<sight::data::transfer_function>();
    parser->createConfig(tf);

    const auto piece = tf->pieces().front();
    CPPUNIT_ASSERT_EQUAL(std::size_t(7), piece->size());

    CPPUNIT_ASSERT_EQUAL(-200., piece->minMax().first);
    CPPUNIT_ASSERT_EQUAL(5000., piece->minMax().second);
    CPPUNIT_ASSERT_EQUAL(5200., piece->window());
    CPPUNIT_ASSERT_EQUAL(2400., piece->level());
    ASSERT_COLOR_EQUALS(data::transfer_function::color_t(1., 1., 0., 1.), piece->sampleLinear(-200));
    ASSERT_COLOR_EQUALS(data::transfer_function::color_t(0., 0., 0., 1.), piece->sampleLinear(0));
    ASSERT_COLOR_EQUALS(data::transfer_function::color_t(0., 0., 1., 1.), piece->sampleLinear(250));
    ASSERT_COLOR_EQUALS(data::transfer_function::color_t(0., 0., 1., 1.), piece->sampleLinear(500));
    ASSERT_COLOR_EQUALS(data::transfer_function::color_t(0., 1., 0., 1.), piece->sampleLinear(1000));
    ASSERT_COLOR_EQUALS(data::transfer_function::color_t(1., 0., 0., 1.), piece->sampleLinear(1500));
    ASSERT_COLOR_EQUALS(data::transfer_function::color_t(0., 0., 0., 1.), piece->sampleLinear(5000));

    CPPUNIT_ASSERT_EQUAL(-200., tf->minMax().first);
    CPPUNIT_ASSERT_EQUAL(5000., tf->minMax().second);
    CPPUNIT_ASSERT_EQUAL(5200., tf->window());
    CPPUNIT_ASSERT_EQUAL(2400., tf->level());
    ASSERT_COLOR_EQUALS(data::transfer_function::color_t(1., 1., 0., 1.), tf->sampleLinear(-200));
    ASSERT_COLOR_EQUALS(data::transfer_function::color_t(0., 0., 0., 1.), tf->sampleLinear(0));
    ASSERT_COLOR_EQUALS(data::transfer_function::color_t(0., 0., 1., 1.), tf->sampleLinear(250));
    ASSERT_COLOR_EQUALS(data::transfer_function::color_t(0., 0., 1., 1.), tf->sampleLinear(500));
    ASSERT_COLOR_EQUALS(data::transfer_function::color_t(0., 1., 0., 1.), tf->sampleLinear(1000));
    ASSERT_COLOR_EQUALS(data::transfer_function::color_t(1., 0., 0., 1.), tf->sampleLinear(1500));
    ASSERT_COLOR_EQUALS(data::transfer_function::color_t(0., 0., 0., 1.), tf->sampleLinear(5000));
}

//------------------------------------------------------------------------------

service::config_t data_parser_test::buildObjectConfig()
{
    service::config_t config;

    // Configuration on core::tools::object which uid is objectUUID
    service::config_t objCfg;
    objCfg.add("<xmlattr>.uid", "objectUUID");
    objCfg.add("<xmlattr>.type", "sight::data::image");
    config.add_child("object", objCfg);

    // Object's service A
    service::config_t serviceA;
    serviceA.add("<xmlattr>.uid", "myTestService1");
    serviceA.add("<xmlattr>.type", "sight::app::ut::STest1Image");

    service::config_t dataServiceA;
    dataServiceA.add("<xmlattr>.key", "data");
    dataServiceA.add("<xmlattr>.uid", "objectUUID");
    serviceA.add_child("in", dataServiceA);
    config.add_child("service", serviceA);

    // Object's service B
    service::config_t serviceB;
    serviceB.add("<xmlattr>.uid", "myTestService2");
    serviceB.add("<xmlattr>.type", "sight::app::ut::STest1Image");
    config.add_child("service", serviceB);

    // Start method from object's services
    service::config_t startA;
    startA.add("<xmlattr>.uid", "myTestService1");
    config.add_child("start", startA);
    service::config_t startB;
    startB.add("<xmlattr>.uid", "myTestService2");
    config.add_child("start", startB);

    // Update method from object's services
    service::config_t update1;
    update1.add("<xmlattr>.uid", "myTestService1");
    config.add_child("update", update1);

    return config;
}

//------------------------------------------------------------------------------

} // namespace sight::app::ut
