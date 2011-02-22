/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <fwTools/Object.hpp>

#include <fwData/Composite.hpp>
#include <fwData/Image.hpp>
#include <fwData/Video.hpp>

#include <fwServices/IService.hpp>
#include <fwServices/IEditionService.hpp>
#include <fwServices/Factory.hpp>
#include <fwServices/Base.hpp>

#include <fwComEd/CompositeMsg.hpp>
#include <fwComEd/CompositeEditor.hpp>
#include <fwComEd/ImageMsg.hpp>
#include <fwComEd/objectConfigurationParser.hpp>

#include "CompositeMessageTest.hpp"

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION( CompositeMessageTest );

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

void CompositeMessageTest::methodeBuildComposite()
{
    const std::string compositeUUID = "compositeUUID";
    const std::string objAUUID = "imageUUID";
    const std::string objBUUID = "videoUUID";
    const std::string serviceUUID1 = "myTestService1";
    const std::string objAType = "::fwData::Image";

    // build composite from ConfigurationElement
    ::boost::shared_ptr< ::fwRuntime::ConfigurationElement > config = buildConfig() ;

    // Create the object and its services from the configuration
    ::fwServices::AppConfigManager::NewSptr configManager;
    configManager->setConfig( config );
    configManager->create();
    ::fwData::Composite::sptr compo = configManager->getConfigRoot< ::fwData::Composite >();

    // test composite
    CPPUNIT_ASSERT_EQUAL(compositeUUID, compo->getID());

    // test composite objects
    CPPUNIT_ASSERT(compo->getRefMap().size() > 0);

    CPPUNIT_ASSERT(compo->getRefMap().find(objAUUID) != compo->getRefMap().end());

    CPPUNIT_ASSERT_EQUAL(objAType, compo->getRefMap()[objAUUID]->className());

    ::fwData::Video::sptr video = ::fwData::Video::dynamicCast(compo->getRefMap()[objBUUID]);
    CPPUNIT_ASSERT_EQUAL(objBUUID, video->getID());

    // test composite services
    ::fwData::Image::sptr image = ::fwData::Image::dynamicCast(compo->getRefMap()[objAUUID]);
    CPPUNIT_ASSERT_EQUAL(objAUUID, image->getID());
    CPPUNIT_ASSERT( ::fwServices::OSR::has(image, "::TestService"));

    CPPUNIT_ASSERT( ::fwServices::OSR::has(compo, "::TestService"));

    /// test start/update/stop service
    configManager->start();
    CPPUNIT_ASSERT(::fwServices::get< ::TestService >(compo, serviceUUID1)->isStarted());

    configManager->update();
    CPPUNIT_ASSERT(::fwServices::get< ::TestService >(compo, serviceUUID1)->getIsUpdated());

    configManager->stop();
    CPPUNIT_ASSERT(::fwServices::get< ::TestService >(compo, serviceUUID1)->isStopped());

    configManager->destroy();
}

//------------------------------------------------------------------------------

void CompositeMessageTest::methodeCompositeMessage()
{
    const std::string objAUUID = "imageUUID";
    const std::string service1UUID = "myTestService1";
    const std::string service2UUID = "myTestService2";

    // build composite
    ::boost::shared_ptr< ::fwRuntime::ConfigurationElement > config = buildConfig() ;

    // Create the object and its services from the configuration
    ::fwServices::AppConfigManager::NewSptr configManager;
    configManager->setConfig( config );
    configManager->create();
    ::fwData::Composite::sptr compo = configManager->getConfigRoot< ::fwData::Composite >();

    ::fwData::Image::sptr image = ::fwData::Image::dynamicCast(compo->getRefMap()[objAUUID]);

    // get service 1
    ::TestService::sptr serviceCompo;
    serviceCompo = ::fwServices::get< ::TestService >(compo, service1UUID);
    CPPUNIT_ASSERT(serviceCompo);

    // get service 2
    ::TestService::sptr serviceCompo2;
    serviceCompo2 = ::fwServices::get< ::TestService >(compo, service2UUID);
    CPPUNIT_ASSERT(serviceCompo2);

    // start services
    configManager->start();
    CPPUNIT_ASSERT(serviceCompo->isStarted());
    CPPUNIT_ASSERT(serviceCompo2->isStarted());

    // register communication channel
    ::fwServices::registerCommunicationChannel(compo, serviceCompo)->start();
    ::fwServices::registerCommunicationChannel(compo, serviceCompo2)->start();

    // notify message
    std::vector< std::string > modifiedFields;
    modifiedFields.push_back(objAUUID);
    ::fwComEd::CompositeMsg::NewSptr compoMsg;
    compoMsg->addEventModifiedFields(modifiedFields);
    ::fwServices::IEditionService::notify(serviceCompo2, compo, compoMsg);

    // test message is received
    CPPUNIT_ASSERT(serviceCompo->getIsUpdatedMessage());
    CPPUNIT_ASSERT(!serviceCompo2->getIsUpdatedMessage());

    ::fwComEd::CompositeMsg::sptr compositeMsg = ::fwComEd::CompositeMsg::dynamicCast(serviceCompo->getMessage());
    CPPUNIT_ASSERT(compositeMsg);

    std::vector< std::string > vEvent = compositeMsg->getEventIds();
    CPPUNIT_ASSERT(std::find(vEvent.begin(), vEvent.end(),::fwComEd::CompositeMsg::MODIFIED_FIELDS) != vEvent.end());

    std::vector< std::string > vModifiedFields = compositeMsg->getEventModifiedFields();
    CPPUNIT_ASSERT(std::find(vModifiedFields.begin(), vModifiedFields.end(),objAUUID) != vModifiedFields.end());

    // unregister communication channel
    ::fwServices::unregisterCommunicationChannel( compo , serviceCompo );
    ::fwServices::unregisterCommunicationChannel(compo, serviceCompo2);

    // stop services
    configManager->stopAndDestroy();
}

//------------------------------------------------------------------------------

void CompositeMessageTest::methodeMessageNotification()
{
    const std::string objAUUID = "imageUUID";
    const std::string service1UUID = "myTestService1";
    const std::string service2UUID = "myTestService2";
    const std::string editionServiceUUID = "myEditionService";
    const std::string ImageServiceUUID = "myImageService";
    const std::string ImageService2UUID = "myImageService2";

    ::boost::shared_ptr< ::fwRuntime::ConfigurationElement > config = buildConfig() ;
    // Create the object and its services from the configuration
    ::fwServices::AppConfigManager::NewSptr configManager;
    configManager->setConfig( config );
    configManager->create();
    ::fwData::Composite::sptr compo = configManager->getConfigRoot< ::fwData::Composite >();

    ::TestService::sptr serviceCompo;
    serviceCompo = ::fwServices::get< ::TestService >(compo);
    CPPUNIT_ASSERT(serviceCompo);

    ::fwData::Image::sptr image = ::fwData::Image::dynamicCast(compo->getRefMap()[objAUUID]);
    ::TestService::sptr serviceImage;
    serviceImage = ::fwServices::get< ::TestService >(image, ImageServiceUUID);
    CPPUNIT_ASSERT(serviceImage);

    ::TestService::sptr serviceImage2;
    serviceImage2 = ::fwServices::get< ::TestService >(image, ImageService2UUID);
    CPPUNIT_ASSERT(serviceImage2);


    // start services
    configManager->start();
    serviceImage->start();
    serviceImage2->start();

    // start communication channel
    ::fwComEd::CompositeEditor::sptr editionService = ::fwComEd::CompositeEditor::dynamicCast(::fwServices::get< ::fwServices::IEditionService >( compo ));
    CPPUNIT_ASSERT_EQUAL(editionServiceUUID, editionService->getID());

    ::fwData::Composite::Container::iterator iter;
    for( iter = compo->getRefMap().begin() ; iter != compo->getRefMap().end(); ++iter )
    {
        fwServices::registerCommunicationChannel( iter->second , editionService )->start();
    }

    ::fwServices::registerCommunicationChannel(image, serviceImage)->start();
    ::fwServices::registerCommunicationChannel(image, serviceImage2)->start();
    ::fwServices::registerCommunicationChannel(compo, serviceCompo)->start();

    // notify message
    ::fwComEd::ImageMsg::sptr imgMsg = ::fwComEd::ImageMsg::New();
    imgMsg->addEvent(::fwComEd::ImageMsg::WINDOWING);

    ::fwServices::IEditionService::notify(serviceImage, image, imgMsg);

    // test receiving message
    CPPUNIT_ASSERT(serviceCompo->getIsUpdatedMessage());
    CPPUNIT_ASSERT(serviceImage2->getIsUpdatedMessage());

    ::fwComEd::CompositeMsg::sptr compositeMsg = ::fwComEd::CompositeMsg::dynamicCast(serviceCompo->getMessage());
    CPPUNIT_ASSERT(compositeMsg);

    ::fwComEd::ImageMsg::sptr imageMsg = ::fwComEd::ImageMsg::dynamicCast(serviceImage2->getMessage());
    CPPUNIT_ASSERT(imageMsg);
    CPPUNIT_ASSERT_EQUAL(imgMsg, imageMsg);

    // stop services
    for( iter = compo->getRefMap().begin() ; iter != compo->getRefMap().end(); ++iter )
    {
        fwServices::unregisterCommunicationChannel( iter->second , editionService );
    }

    serviceImage->stop();
    configManager->stopAndDestroy();
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
    imageService->setAttributeValue( "type" , "::TestService" ) ;
    imageService->setAttributeValue( "implementation" , "::TestServiceImplementationImage" ) ;
    imageService->setAttributeValue( "autoComChannel" , "no" ) ;

    ::boost::shared_ptr< ::fwRuntime::EConfigurationElement > imageService2 = objA->addConfigurationElement("service");
    imageService2->setAttributeValue( "uid" , "myImageService2" ) ;
    imageService2->setAttributeValue( "type" , "::TestService" ) ;
    imageService2->setAttributeValue( "implementation" , "::TestServiceImplementationImage" ) ;
    imageService2->setAttributeValue( "autoComChannel" , "no" ) ;

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
    service->setAttributeValue( "type" , "::TestService" ) ;
    service->setAttributeValue( "implementation" , "::TestServiceImplementationComposite" ) ;
    service->setAttributeValue( "autoComChannel" , "no" ) ;

    // start / stop / update on service 1
    ::boost::shared_ptr< ::fwRuntime::EConfigurationElement > start = cfg->addConfigurationElement("start");
    start->setAttributeValue( "uid" , "myTestService1" ) ;
    ::boost::shared_ptr< ::fwRuntime::EConfigurationElement > update = cfg->addConfigurationElement("update");
    update->setAttributeValue( "uid" , "myTestService1" ) ;
    ::boost::shared_ptr< ::fwRuntime::EConfigurationElement > stop = cfg->addConfigurationElement("stop");
    stop->setAttributeValue( "uid" , "myTestService1" ) ;

    // composite's service 2
    ::boost::shared_ptr< ::fwRuntime::EConfigurationElement > service2 = cfg->addConfigurationElement("service");
    service2->setAttributeValue( "uid" , "myTestService2" ) ;
    service2->setAttributeValue( "type" , "::TestService" ) ;
    service2->setAttributeValue( "implementation" , "::TestServiceImplementationComposite" ) ;
    service2->setAttributeValue( "autoComChannel" , "no" ) ;

    // start / stop / update on service 2
    ::boost::shared_ptr< ::fwRuntime::EConfigurationElement > start2 = cfg->addConfigurationElement("start");
    start2->setAttributeValue( "uid" , "myTestService2" ) ;
    ::boost::shared_ptr< ::fwRuntime::EConfigurationElement > update2 = cfg->addConfigurationElement("update");
    update2->setAttributeValue( "uid" , "myTestService2" ) ;
    ::boost::shared_ptr< ::fwRuntime::EConfigurationElement > stop2 = cfg->addConfigurationElement("stop");
    stop2->setAttributeValue( "uid" , "myTestService2" ) ;

    // composite's edition service
//    ::boost::shared_ptr< ::fwRuntime::EConfigurationElement > editionService = cfg->addConfigurationElement("service");
//    editionService->setAttributeValue( "uid" , "myEditionService" ) ;
//    editionService->setAttributeValue( "type" , "::fwServices::IEditionService" ) ;
//    editionService->setAttributeValue( "implementation" , "::fwComEd::CompositeEditor" ) ;
//    editionService->setAttributeValue( "autoComChannel" , "no" ) ;

    return cfg ;
}

//------------------------------------------------------------------------------
