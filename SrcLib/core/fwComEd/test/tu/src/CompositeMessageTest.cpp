/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2012.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <fwData/Composite.hpp>
#include <fwData/Image.hpp>
#include <fwData/Video.hpp>

#include <fwServices/IService.hpp>
#include <fwServices/IEditionService.hpp>
#include <fwServices/Base.hpp>
#include <fwServices/AppConfigManager.hpp>
#include <fwServices/registry/ActiveWorkers.hpp>

#include <fwComEd/CompositeMsg.hpp>
#include <fwComEd/ImageMsg.hpp>

#include <fwTest/Exception.hpp>

#include "ConfigParserTest.hpp"
#include "CompositeMessageTest.hpp"

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION( ::fwComEd::ut::CompositeMessageTest );

namespace fwComEd
{
namespace ut
{

static ::fwTest::Exception fwTestException(""); // force link with fwTest

//------------------------------------------------------------------------------

void CompositeMessageTest::setUp()
{
    // Set up context before running a test.
}

//------------------------------------------------------------------------------

void CompositeMessageTest::tearDown()
{
    // Clean up after the test run.
}

//------------------------------------------------------------------------------

void CompositeMessageTest::testCompositeMessage()
{
    ::fwServices::registry::ActiveWorkers::sptr activeWorkers = ::fwServices::registry::ActiveWorkers::getDefault();
    activeWorkers->initRegistry();

    const std::string objAUUID = "imageUUID";
    const std::string service1UUID = "myTestService1";
    const std::string service2UUID = "myTestService2";

    // build composite
    ::fwRuntime::ConfigurationElement::sptr config = buildConfig() ;

    // Create the object and its services from the configuration
    ::fwServices::AppConfigManager::NewSptr configManager;
    configManager->setConfig( config );
    configManager->create();
    ::fwData::Composite::sptr compo = configManager->getConfigRoot< ::fwData::Composite >();

    ::fwData::Image::sptr image = ::fwData::Image::dynamicCast(compo->getContainer()[objAUUID]);

    // get service 1
    ::fwComEd::ut::TestService::sptr serviceCompo;
    serviceCompo = ::fwComEd::ut::TestService::dynamicCast( ::fwServices::get(service1UUID) );
    CPPUNIT_ASSERT(serviceCompo);

    // get service 2
    ::fwComEd::ut::TestService::sptr serviceCompo2;
    serviceCompo2 = ::fwComEd::ut::TestService::dynamicCast( ::fwServices::get(service2UUID) );
    CPPUNIT_ASSERT(serviceCompo2);

    // start services
    configManager->start();
    CPPUNIT_ASSERT(serviceCompo->isStarted());
    CPPUNIT_ASSERT(serviceCompo2->isStarted());

    // register communication channel
    ::fwServices::helper::SigSlotConnection::NewSptr helper;
    helper->connect( compo, serviceCompo, serviceCompo->getObjSrvConnections() );
    helper->connect( compo, serviceCompo2, serviceCompo2->getObjSrvConnections() );

    // notify message
    std::vector< std::string > modifiedFields;
    modifiedFields.push_back(objAUUID);
    ::fwComEd::CompositeMsg::NewSptr compoMsg;
    compoMsg->addModifiedKeysEvent(modifiedFields);
    ::fwServices::IEditionService::notify(serviceCompo2, compo, compoMsg);

    // Wait a little notification system
    ::boost::this_thread::sleep( ::boost::posix_time::milliseconds(500) );

    // test message is received
    CPPUNIT_ASSERT(serviceCompo->getIsUpdatedMessage());
    CPPUNIT_ASSERT(!serviceCompo2->getIsUpdatedMessage());

    ::fwComEd::CompositeMsg::sptr compositeMsg = ::fwComEd::CompositeMsg::dynamicCast(serviceCompo->getMessage());
    CPPUNIT_ASSERT(compositeMsg);

    std::vector< std::string > vEvent = compositeMsg->getEventIds();
    CPPUNIT_ASSERT(std::find(vEvent.begin(), vEvent.end(),::fwComEd::CompositeMsg::MODIFIED_KEYS) != vEvent.end());

    std::vector< std::string > vModifiedFields = compositeMsg->getModifiedKeys();
    CPPUNIT_ASSERT(std::find(vModifiedFields.begin(), vModifiedFields.end(),objAUUID) != vModifiedFields.end());

    // unregister communication channel
    helper->disconnect();
    helper.reset();

    // stop services
    configManager->stopAndDestroy();

    activeWorkers->clearRegistry();
}

//------------------------------------------------------------------------------

::boost::shared_ptr< ::fwRuntime::ConfigurationElement > CompositeMessageTest::buildConfig()
{
    // Composite
    ::boost::shared_ptr< ::fwRuntime::EConfigurationElement > cfg ( new ::fwRuntime::EConfigurationElement("object")) ;
    cfg->setAttributeValue( "uid" , "compositeUUID") ;
    cfg->setAttributeValue( "type" , "::fwData::Composite") ;


    ::boost::shared_ptr< ::fwRuntime::EConfigurationElement > itemA = cfg->addConfigurationElement("item");
     itemA->setAttributeValue( "key" , "imageUUID") ;


    // composite object : image
    ::boost::shared_ptr< ::fwRuntime::EConfigurationElement > objA = itemA->addConfigurationElement("object");
    objA->setAttributeValue( "uid" , "imageUUID") ;
    objA->setAttributeValue( "type" , "::fwData::Image") ;

    // image's services
    ::boost::shared_ptr< ::fwRuntime::EConfigurationElement > imageService = objA->addConfigurationElement("service");
    imageService->setAttributeValue( "uid" , "myImageService" ) ;
    imageService->setAttributeValue( "impl" , "::fwComEd::ut::TestServiceImplementationImage" ) ;

    ::boost::shared_ptr< ::fwRuntime::EConfigurationElement > imageService2 = objA->addConfigurationElement("service");
    imageService2->setAttributeValue( "uid" , "myImageService2" ) ;
    imageService2->setAttributeValue( "impl" , "::fwComEd::ut::TestServiceImplementationImage" ) ;

    ::boost::shared_ptr< ::fwRuntime::EConfigurationElement > itemB = cfg->addConfigurationElement("item");
     itemB->setAttributeValue( "key" , "videoUUID") ;


    // composite object : video
    ::boost::shared_ptr< ::fwRuntime::EConfigurationElement > objB = itemB->addConfigurationElement("object");
    objB->setAttributeValue( "uid" , "videoUUID") ;
    //objB->setAttributeValue( "id" , "videoUUID") ;
    objB->setAttributeValue( "type" , "::fwData::Video") ;

    // composite's service 1
    ::boost::shared_ptr< ::fwRuntime::EConfigurationElement > service = cfg->addConfigurationElement("service");
    service->setAttributeValue( "uid" , "myTestService1" ) ;
    service->setAttributeValue( "impl" , "::fwComEd::ut::TestServiceImplementationComposite" ) ;

    // start / stop / update on service 1
    ::boost::shared_ptr< ::fwRuntime::EConfigurationElement > start = cfg->addConfigurationElement("start");
    start->setAttributeValue( "uid" , "myTestService1" ) ;
    ::boost::shared_ptr< ::fwRuntime::EConfigurationElement > update = cfg->addConfigurationElement("update");
    update->setAttributeValue( "uid" , "myTestService1" ) ;

    // composite's service 2
    ::boost::shared_ptr< ::fwRuntime::EConfigurationElement > service2 = cfg->addConfigurationElement("service");
    service2->setAttributeValue( "uid" , "myTestService2" ) ;
    service2->setAttributeValue( "impl" , "::fwComEd::ut::TestServiceImplementationComposite" ) ;

    // start / stop / update on service 2
    ::boost::shared_ptr< ::fwRuntime::EConfigurationElement > start2 = cfg->addConfigurationElement("start");
    start2->setAttributeValue( "uid" , "myTestService2" ) ;
    ::boost::shared_ptr< ::fwRuntime::EConfigurationElement > update2 = cfg->addConfigurationElement("update");
    update2->setAttributeValue( "uid" , "myTestService2" ) ;

    return cfg ;
}

//------------------------------------------------------------------------------

} //namespace ut
} //namespace fwComEd
