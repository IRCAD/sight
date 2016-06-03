/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <fwData/Composite.hpp>
#include <fwData/Image.hpp>
#include <fwData/Mesh.hpp>

#include <fwServices/IService.hpp>
#include <fwServices/macros.hpp>
#include <fwServices/op/Get.hpp>
#include <fwServices/AppConfigManager.hpp>

#include "TestServices.hpp"
#include "ConfigParserTest.hpp"

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION( ::fwComEd::ut::ConfigParserTest );

//------------------------------------------------------------------------------

namespace fwComEd
{
namespace ut
{

void ConfigParserTest::setUp()
{
    // Set up context before running a test.
}

//------------------------------------------------------------------------------

void ConfigParserTest::tearDown()
{
    // Clean up after the test run.
}

//------------------------------------------------------------------------------

void ConfigParserTest::testObjectCreationWithConfig()
{
    const std::string objectUUID   = "objectUUID";
    const std::string serviceUUID1 = "myTestService1";
    const std::string serviceUUID2 = "myTestService2";

    // Create object configuration
    ::fwRuntime::ConfigurationElement::sptr config = buildObjectConfig();

    // Create the object and its services from the configuration
    ::fwServices::AppConfigManager::sptr configManager = ::fwServices::AppConfigManager::New();
    configManager->::fwServices::IAppConfigManager::setConfig( config );
    configManager->create();
    auto image = ::fwData::Image::dynamicCast(configManager->getConfigRoot());

    // Test object uid
    CPPUNIT_ASSERT_EQUAL(objectUUID, image->getID());

    // Test if object's service is created
    CPPUNIT_ASSERT( ::fwServices::OSR::has(image, "::fwComEd::ut::TestService"));

    // Test start services
    configManager->start();
    CPPUNIT_ASSERT( ::fwServices::get(serviceUUID1)->isStarted() );
    CPPUNIT_ASSERT( ::fwServices::get(serviceUUID2)->isStarted() );

    // Test update services
    configManager->update();
    CPPUNIT_ASSERT( ::fwComEd::ut::TestService::dynamicCast( ::fwServices::get(serviceUUID1) )->getIsUpdated() );
    CPPUNIT_ASSERT( ::fwComEd::ut::TestService::dynamicCast( ::fwServices::get(serviceUUID2) )->getIsUpdated() ==
                    false );

    // Test stop services
    configManager->stop();
    CPPUNIT_ASSERT( ::fwServices::get(serviceUUID1)->isStopped() );
    CPPUNIT_ASSERT( ::fwServices::get(serviceUUID2)->isStopped() );

    configManager->destroy();
}

//------------------------------------------------------------------------------

void ConfigParserTest::testBuildComposite()
{
    const std::string compositeUUID = "compositeUUID";
    const std::string objAUUID      = "imageUUID";
    const std::string objBUUID      = "meshUUID";
    const std::string serviceUUID1  = "myTestService1";
    const std::string objAType      = "::fwData::Image";

    // build composite from ConfigurationElement
    std::shared_ptr< ::fwRuntime::ConfigurationElement > config = buildCompositeConfig();

    // Create the object and its services from the configuration
    ::fwServices::AppConfigManager::sptr configManager = ::fwServices::AppConfigManager::New();
    configManager->::fwServices::IAppConfigManager::setConfig( config );
    configManager->create();
    auto compo = ::fwData::Composite::dynamicCast(configManager->getConfigRoot());

    // test composite
    CPPUNIT_ASSERT_EQUAL(compositeUUID, compo->getID());

    // test composite objects
    CPPUNIT_ASSERT(compo->getContainer().size() > 0);

    CPPUNIT_ASSERT(compo->find(objAUUID) != compo->end());

    CPPUNIT_ASSERT_EQUAL(objAType, compo->getContainer()[objAUUID]->className());

    ::fwData::Mesh::sptr mesh = ::fwData::Mesh::dynamicCast(compo->getContainer()[objBUUID]);
    CPPUNIT_ASSERT_EQUAL(objBUUID, mesh->getID());

    // test composite services
    ::fwData::Image::sptr image = ::fwData::Image::dynamicCast(compo->getContainer()[objAUUID]);
    CPPUNIT_ASSERT_EQUAL(objAUUID, image->getID());
    CPPUNIT_ASSERT( ::fwServices::OSR::has(image, "::fwComEd::ut::TestService"));

    CPPUNIT_ASSERT( ::fwServices::OSR::has(compo, "::fwComEd::ut::TestService"));

    /// test start/update/stop service
    configManager->start();
    CPPUNIT_ASSERT(::fwServices::get(serviceUUID1)->isStarted());

    configManager->update();
    CPPUNIT_ASSERT(::fwComEd::ut::TestService::dynamicCast(::fwServices::get(serviceUUID1))->getIsUpdated());

    configManager->stop();
    CPPUNIT_ASSERT(::fwServices::get(serviceUUID1)->isStopped());

    configManager->destroy();
}

//------------------------------------------------------------------------------

::fwRuntime::ConfigurationElement::sptr ConfigParserTest::buildObjectConfig()
{
    // Configuration on fwTools::Object which uid is objectUUID
    std::shared_ptr< ::fwRuntime::EConfigurationElement > cfg ( new ::fwRuntime::EConfigurationElement("object"));
    cfg->setAttributeValue( "uid", "objectUUID");
    cfg->setAttributeValue( "type", "::fwData::Image");

    // Object's service A
    std::shared_ptr< ::fwRuntime::EConfigurationElement > serviceA = cfg->addConfigurationElement("service");
    serviceA->setAttributeValue( "uid", "myTestService1" );
    serviceA->setAttributeValue( "type", "::fwComEd::ut::TestService" );
    serviceA->setAttributeValue( "impl", "::fwComEd::ut::TestServiceImplementationImage" );

    // Object's service B
    std::shared_ptr< ::fwRuntime::EConfigurationElement > serviceB = cfg->addConfigurationElement("service");
    serviceB->setAttributeValue( "uid", "myTestService2" );
    serviceB->setAttributeValue( "impl", "::fwComEd::ut::TestServiceImplementationImage" );

    // Start method from object's services
    std::shared_ptr< ::fwRuntime::EConfigurationElement > startA = cfg->addConfigurationElement("start");
    startA->setAttributeValue( "uid", "myTestService1" );
    std::shared_ptr< ::fwRuntime::EConfigurationElement > startB = cfg->addConfigurationElement("start");
    startB->setAttributeValue( "uid", "myTestService2" );

    // Update method from object's services
    std::shared_ptr< ::fwRuntime::EConfigurationElement > updateA = cfg->addConfigurationElement("update");
    updateA->setAttributeValue( "uid", "myTestService1" );

    return cfg;
}

//------------------------------------------------------------------------------

::fwRuntime::ConfigurationElement::sptr ConfigParserTest::buildCompositeConfig()
{
    // Composite
    std::shared_ptr< ::fwRuntime::EConfigurationElement > cfg ( new ::fwRuntime::EConfigurationElement("object"));
    cfg->setAttributeValue( "uid", "compositeUUID");
    cfg->setAttributeValue( "type", "::fwData::Composite");


    std::shared_ptr< ::fwRuntime::EConfigurationElement > itemA = cfg->addConfigurationElement("item");
    itemA->setAttributeValue( "key", "imageUUID");


    // composite object : image
    std::shared_ptr< ::fwRuntime::EConfigurationElement > objA = itemA->addConfigurationElement("object");
    objA->setAttributeValue( "uid", "imageUUID");
    objA->setAttributeValue( "type", "::fwData::Image");

    // image's services
    std::shared_ptr< ::fwRuntime::EConfigurationElement > imageService = objA->addConfigurationElement("service");
    imageService->setAttributeValue( "uid", "myImageService" );
    imageService->setAttributeValue( "impl", "::fwComEd::ut::TestServiceImplementationImage" );

    std::shared_ptr< ::fwRuntime::EConfigurationElement > imageService2 = objA->addConfigurationElement("service");
    imageService2->setAttributeValue( "uid", "myImageService2" );
    imageService2->setAttributeValue( "impl", "::fwComEd::ut::TestServiceImplementationImage" );

    std::shared_ptr< ::fwRuntime::EConfigurationElement > itemB = cfg->addConfigurationElement("item");
    itemB->setAttributeValue( "key", "meshUUID");


    // composite object : mesh
    std::shared_ptr< ::fwRuntime::EConfigurationElement > objB = itemB->addConfigurationElement("object");
    objB->setAttributeValue( "uid", "meshUUID");
    //objB->setAttributeValue( "id" , "meshUUID") ;
    objB->setAttributeValue( "type", "::fwData::Mesh");

    // composite's service 1
    std::shared_ptr< ::fwRuntime::EConfigurationElement > service = cfg->addConfigurationElement("service");
    service->setAttributeValue( "uid", "myTestService1" );
    service->setAttributeValue( "impl", "::fwComEd::ut::TestServiceImplementationComposite" );

    // start / stop / update on service 1
    std::shared_ptr< ::fwRuntime::EConfigurationElement > start = cfg->addConfigurationElement("start");
    start->setAttributeValue( "uid", "myTestService1" );
    std::shared_ptr< ::fwRuntime::EConfigurationElement > update = cfg->addConfigurationElement("update");
    update->setAttributeValue( "uid", "myTestService1" );

    // composite's service 2
    std::shared_ptr< ::fwRuntime::EConfigurationElement > service2 = cfg->addConfigurationElement("service");
    service2->setAttributeValue( "uid", "myTestService2" );
    service2->setAttributeValue( "impl", "::fwComEd::ut::TestServiceImplementationComposite" );

    // start / stop / update on service 2
    std::shared_ptr< ::fwRuntime::EConfigurationElement > start2 = cfg->addConfigurationElement("start");
    start2->setAttributeValue( "uid", "myTestService2" );
    std::shared_ptr< ::fwRuntime::EConfigurationElement > update2 = cfg->addConfigurationElement("update");
    update2->setAttributeValue( "uid", "myTestService2" );

    return cfg;
}

//------------------------------------------------------------------------------

} //namespace ut
} //namespace fwComEd
