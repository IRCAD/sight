/************************************************************************
 *
 * Copyright (C) 2009-2022 IRCAD France
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

#include "IoPacsTest.hpp"

#include <io/dimse/data/PacsConfiguration.hpp>

#include <service/AppConfigManager.hpp>
#include <service/extension/AppConfig.hpp>
#include <service/macros.hpp>
#include <service/op/Add.hpp>
#include <service/registry/ObjectService.hpp>

#include <boost/property_tree/xml_parser.hpp>

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION(sight::module::io::dimse::ut::IoPacsTest);

namespace sight::module::io::dimse::ut
{

//------------------------------------------------------------------------------

void IoPacsTest::setUp()
{
    // Set up context before running a test.
}

//------------------------------------------------------------------------------

void IoPacsTest::tearDown()
{
    // Clean up after the test run.
}

//------------------------------------------------------------------------------

void IoPacsTest::pacsConfigurationInitializer()
{
    // Pacs Configuration
    sight::io::dimse::data::PacsConfiguration::sptr pacsConfiguration =
        sight::io::dimse::data::PacsConfiguration::New();

    // Create service
    service::IService::sptr srv = service::add("sight::module::io::dimse::SPacsConfigurationInitializer");
    CPPUNIT_ASSERT(srv);

    // Create service configuration
    service::IService::ConfigType configSrv;
    std::stringstream config_string;
    config_string << "<config localApplicationTitle=\"VRRender\""
                     "pacsHostName=\"mypacs.mycompany.com\""
                     "pacsApplicationTitle=\"PACSNAME\""
                     "pacsApplicationPort=\"11112\""
                     "moveApplicationTitle=\"MoveApplicationTitle\""
                     "moveApplicationPort=\"11110\""
                     "retrieveMethod=\"GET\""
                     "/>";
    boost::property_tree::read_xml(config_string, configSrv);
    // Use the service
    srv->setInOut(pacsConfiguration, "config");
    srv->setConfiguration(configSrv);
    srv->configure();
    srv->start();
    srv->update();
    srv->stop();
    service::OSR::unregisterService(srv);

    std::uint16_t pacsApplicationPort = 11112U;
    std::uint16_t moveApplicationPort = 11110U;
    CPPUNIT_ASSERT_EQUAL(std::string("VRRender"), pacsConfiguration->getLocalApplicationTitle());
    CPPUNIT_ASSERT_EQUAL(std::string("mypacs.mycompany.com"), pacsConfiguration->getPacsHostName());
    CPPUNIT_ASSERT_EQUAL(std::string("PACSNAME"), pacsConfiguration->getPacsApplicationTitle());
    CPPUNIT_ASSERT_EQUAL(pacsApplicationPort, pacsConfiguration->getPacsApplicationPort());
    CPPUNIT_ASSERT_EQUAL(std::string("MoveApplicationTitle"), pacsConfiguration->getMoveApplicationTitle());
    CPPUNIT_ASSERT_EQUAL(moveApplicationPort, pacsConfiguration->getMoveApplicationPort());
    CPPUNIT_ASSERT_EQUAL(
        sight::io::dimse::data::PacsConfiguration::GET_RETRIEVE_METHOD,
        pacsConfiguration->getRetrieveMethod()
    );
}

//------------------------------------------------------------------------------

} // namespace sight::module::io::dimse::ut
