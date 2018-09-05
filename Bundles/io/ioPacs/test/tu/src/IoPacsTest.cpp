/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2018.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "IoPacsTest.hpp"

#include <fwPacsIO/data/PacsConfiguration.hpp>

#include <fwRuntime/EConfigurationElement.hpp>
#include <fwRuntime/profile/Profile.hpp>

#include <fwServices/AppConfigManager.hpp>
#include <fwServices/macros.hpp>
#include <fwServices/op/Add.hpp>
#include <fwServices/registry/AppConfig.hpp>
#include <fwServices/registry/ObjectService.hpp>

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION( ::ioPacs::ut::IoPacsTest );

namespace ioPacs
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
    ::fwPacsIO::data::PacsConfiguration::sptr pacsConfiguration = ::fwPacsIO::data::PacsConfiguration::New();

    // Create service
    ::fwServices::IService::sptr srv = ::fwServices::add( "::ioPacs::SPacsConfigurationInitializer" );
    CPPUNIT_ASSERT(srv);

    // Create service configuration
    ::fwRuntime::EConfigurationElement::sptr srvElement    = ::fwRuntime::EConfigurationElement::New("service");
    ::fwRuntime::EConfigurationElement::sptr configElement = ::fwRuntime::EConfigurationElement::New("config");
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
    ::fwServices::OSR::unregisterService( srv );

    unsigned short pacsApplicationPort = 11112u;
    unsigned short moveApplicationPort = 11110u;
    CPPUNIT_ASSERT_EQUAL(std::string("VRRender"), pacsConfiguration->getLocalApplicationTitle());
    CPPUNIT_ASSERT_EQUAL(std::string("mypacs.mycompany.com"), pacsConfiguration->getPacsHostName());
    CPPUNIT_ASSERT_EQUAL(std::string("PACSNAME"), pacsConfiguration->getPacsApplicationTitle());
    CPPUNIT_ASSERT_EQUAL(pacsApplicationPort, pacsConfiguration->getPacsApplicationPort());
    CPPUNIT_ASSERT_EQUAL(std::string("MoveApplicationTitle"), pacsConfiguration->getMoveApplicationTitle());
    CPPUNIT_ASSERT_EQUAL(moveApplicationPort, pacsConfiguration->getMoveApplicationPort());
    CPPUNIT_ASSERT_EQUAL(::fwPacsIO::data::PacsConfiguration::GET_RETRIEVE_METHOD,
                         pacsConfiguration->getRetrieveMethod());

}

//------------------------------------------------------------------------------

} //namespace ut
} //namespace ioPacs
