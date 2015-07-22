/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2015.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <fwRuntime/EConfigurationElement.hpp>
#include <fwRuntime/profile/Profile.hpp>

#include <fwServices/Base.hpp>
#include <fwServices/AppConfigManager.hpp>
#include <fwServices/registry/AppConfig.hpp>

#include <fwDicomIOExt/data/PacsConfiguration.hpp>

#include "IoDicomExtTest.hpp"

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION( ::ioDicomExt::ut::IoDicomExtTest );


namespace ioDicomExt
{
namespace ut
{

//------------------------------------------------------------------------------

void IoDicomExtTest::setUp()
{
    // Set up context before running a test.
}

//------------------------------------------------------------------------------

void IoDicomExtTest::tearDown()
{
    // Clean up after the test run.
}

//------------------------------------------------------------------------------

void IoDicomExtTest::pacsConfigurationInitializer()
{
    // Pacs Conifguration
    ::fwDicomIOExt::data::PacsConfiguration::sptr pacsConfiguration = ::fwDicomIOExt::data::PacsConfiguration::New();

    // Create service
    ::fwServices::IService::sptr srv =
        ::fwServices::registry::ServiceFactory::getDefault()->create( "::fwServices::IController",
                                                                      "::ioDicomExt::common::controller::SPacsConfigurationInitializer" );
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
    ::fwServices::OSR::registerService( pacsConfiguration, srv );
    srv->setConfiguration( srvElement );
    srv->configure();
    srv->start();
    srv->update();
    srv->stop();
    ::fwServices::OSR::unregisterService( srv );

    CPPUNIT_ASSERT_EQUAL(std::string("VRRender"), pacsConfiguration->getLocalApplicationTitle());
    CPPUNIT_ASSERT_EQUAL(std::string("mypacs.mycompany.com"), pacsConfiguration->getPacsHostName());
    CPPUNIT_ASSERT_EQUAL(std::string("PACSNAME"), pacsConfiguration->getPacsApplicationTitle());
    CPPUNIT_ASSERT_EQUAL(11112u, pacsConfiguration->getPacsApplicationPort());
    CPPUNIT_ASSERT_EQUAL(std::string("MoveApplicationTitle"), pacsConfiguration->getMoveApplicationTitle());
    CPPUNIT_ASSERT_EQUAL(11110u, pacsConfiguration->getMoveApplicationPort());
    CPPUNIT_ASSERT_EQUAL(::fwDicomIOExt::data::PacsConfiguration::GET_RETRIEVE_METHOD,
                         pacsConfiguration->getRetrieveMethod());

}


//------------------------------------------------------------------------------

} //namespace ut
} //namespace ioDicomExt
