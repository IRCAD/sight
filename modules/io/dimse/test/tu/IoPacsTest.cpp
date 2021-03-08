/************************************************************************
 *
 * Copyright (C) 2009-2021 IRCAD France
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

#include <core/runtime/EConfigurationElement.hpp>

#include <service/AppConfigManager.hpp>
#include <service/macros.hpp>
#include <service/op/Add.hpp>
#include <service/extension/AppConfig.hpp>
#include <service/registry/ObjectService.hpp>

#include <io/dimse/data/PacsConfiguration.hpp>

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION( ::sight::module::io::dimse::ut::IoPacsTest );

namespace sight::module::io::dimse
{
namespace ut
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
    // Pacs Conifguration
    sight::io::dimse::data::PacsConfiguration::sptr pacsConfiguration =
        sight::io::dimse::data::PacsConfiguration::New();

    // Create service
    service::IService::sptr srv = service::add( "::sight::module::io::dimse::SPacsConfigurationInitializer" );
    CPPUNIT_ASSERT(srv);

    // Create service configuration
    core::runtime::EConfigurationElement::sptr srvElement    = core::runtime::EConfigurationElement::New("service");
    core::runtime::EConfigurationElement::sptr configElement = core::runtime::EConfigurationElement::New("config");
    srvElement->addConfigurationElement(configElement);
    configElement->setAttributeValue("localApplicationTitle", "VRRender");
    configElement->setAttributeValue("pacsHostName", "mypacs.mycompany.com");
    configElement->setAttributeValue("pacsApplicationTitle", "PACSNAME");
    configElement->setAttributeValue("pacsApplicationPort", "11112");
    configElement->setAttributeValue("moveApplicationTitle", "MoveApplicationTitle");
    configElement->setAttributeValue("moveApplicationPort", "11110");
    configElement->setAttributeValue("retrieveMethod", "GET");

    // Use the service
    srv->registerInOut(pacsConfiguration, "config" );
    srv->setConfiguration( srvElement );
    srv->configure();
    srv->start();
    srv->update();
    srv->stop();
    service::OSR::unregisterService( srv );

    unsigned short pacsApplicationPort = 11112u;
    unsigned short moveApplicationPort = 11110u;
    CPPUNIT_ASSERT_EQUAL(std::string("VRRender"), pacsConfiguration->getLocalApplicationTitle());
    CPPUNIT_ASSERT_EQUAL(std::string("mypacs.mycompany.com"), pacsConfiguration->getPacsHostName());
    CPPUNIT_ASSERT_EQUAL(std::string("PACSNAME"), pacsConfiguration->getPacsApplicationTitle());
    CPPUNIT_ASSERT_EQUAL(pacsApplicationPort, pacsConfiguration->getPacsApplicationPort());
    CPPUNIT_ASSERT_EQUAL(std::string("MoveApplicationTitle"), pacsConfiguration->getMoveApplicationTitle());
    CPPUNIT_ASSERT_EQUAL(moveApplicationPort, pacsConfiguration->getMoveApplicationPort());
    CPPUNIT_ASSERT_EQUAL(sight::io::dimse::data::PacsConfiguration::GET_RETRIEVE_METHOD,
                         pacsConfiguration->getRetrieveMethod());

}

//------------------------------------------------------------------------------

} //namespace ut
} //namespace sight::module::io::dimse
