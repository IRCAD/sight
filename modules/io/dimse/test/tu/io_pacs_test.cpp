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

#include "io_pacs_test.hpp"

#include <io/dimse/data/pacs_configuration.hpp>

#include <service/op.hpp>

#include <boost/property_tree/xml_parser.hpp>

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION(sight::module::io::dimse::ut::io_pacs_test);

namespace sight::module::io::dimse::ut
{

//------------------------------------------------------------------------------

void io_pacs_test::setUp()
{
    // Set up context before running a test.
}

//------------------------------------------------------------------------------

void io_pacs_test::tearDown()
{
    // Clean up after the test run.
}

//------------------------------------------------------------------------------

void io_pacs_test::pacs_configuration_initializer()
{
    // Pacs Configuration
    auto pacs_configuration = std::make_shared<sight::io::dimse::data::pacs_configuration>();

    // Create service
    service::base::sptr srv = service::add("sight::module::io::dimse::pacs_configuration_initializer");
    CPPUNIT_ASSERT(srv);

    // Create service configuration
    service::config_t config_srv;
    std::stringstream config_string;
    config_string << "<config localApplicationTitle=\"VRRender\""
                     "pacsHostName=\"mypacs.mycompany.com\""
                     "pacsApplicationTitle=\"PACSNAME\""
                     "pacsApplicationPort=\"11112\""
                     "moveApplicationTitle=\"MoveApplicationTitle\""
                     "moveApplicationPort=\"11110\""
                     "retrieveMethod=\"GET\""
                     "/>";
    boost::property_tree::read_xml(config_string, config_srv);
    // Use the service
    srv->set_inout(pacs_configuration, "config");
    srv->set_config(config_srv);
    srv->configure();
    srv->start();
    srv->update();
    srv->stop();
    service::unregister_service(srv);

    std::uint16_t pacs_application_port = 11112U;
    std::uint16_t move_application_port = 11110U;
    CPPUNIT_ASSERT_EQUAL(std::string("VRRender"), pacs_configuration->get_local_application_title());
    CPPUNIT_ASSERT_EQUAL(std::string("mypacs.mycompany.com"), pacs_configuration->get_pacs_host_name());
    CPPUNIT_ASSERT_EQUAL(std::string("PACSNAME"), pacs_configuration->get_pacs_application_title());
    CPPUNIT_ASSERT_EQUAL(pacs_application_port, pacs_configuration->get_pacs_application_port());
    CPPUNIT_ASSERT_EQUAL(std::string("MoveApplicationTitle"), pacs_configuration->get_move_application_title());
    CPPUNIT_ASSERT_EQUAL(move_application_port, pacs_configuration->get_move_application_port());
    CPPUNIT_ASSERT_EQUAL(
        sight::io::dimse::data::pacs_configuration::retrieve_method::get,
        pacs_configuration->get_retrieve_method()
    );
}

//------------------------------------------------------------------------------

} // namespace sight::module::io::dimse::ut
