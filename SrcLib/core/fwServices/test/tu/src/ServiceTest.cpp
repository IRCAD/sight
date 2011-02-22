/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <fwTools/Object.hpp>
#include <fwTools/fwID.hpp>
#include <fwTools/Factory.hpp>

#include <fwData/Composite.hpp>
#include <fwData/Image.hpp>

#include <fwServices/IService.hpp>
#include <fwServices/IEditionService.hpp>
#include <fwServices/Factory.hpp>
#include <fwServices/Base.hpp>
#include <fwServices/macros.hpp>
#include <fwServices/registry/ServiceFactory.hpp>
#include <fwServices/AppConfigManager.hpp>

#include <fwRuntime/EConfigurationElement.hpp>
#include <fwRuntime/helper.hpp>

#include "ServiceTest.hpp"

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION( ServiceTest );

//------------------------------------------------------------------------------

void ServiceTest::setUp()
{
    // Set up context before running a test.
}

//------------------------------------------------------------------------------

void ServiceTest::tearDown()
{
    // Clean up after the test run.
}

//------------------------------------------------------------------------------

void ServiceTest::testServiceCreation()
{
    ::fwTools::Object::sptr obj = ::fwTools::Object::New();
    ::fwServices::IService::sptr service;

    // Test if the object support the service
    CPPUNIT_ASSERT( ::fwServices::registry::ServiceFactory::getDefault()->support(obj->getClassname(), "::TestService") );

    // Test adding service
    ::fwServices::add(obj, "::TestService", "::TestServiceImplementation");
    CPPUNIT_ASSERT(::fwServices::OSR::has(obj, "::TestService") );

    // Test getting the service its object
    service = ::fwServices::get(obj, "::TestService");
    CPPUNIT_ASSERT(service);
    CPPUNIT_ASSERT_EQUAL(obj, service->getObject< ::fwTools::Object >());

    // Test erasing service
    ::fwServices::OSR::unregisterService(service);
    CPPUNIT_ASSERT( ::fwServices::OSR::has(obj, "::TestService") == false );
}

//------------------------------------------------------------------------------

void ServiceTest::testServiceCreationWithTemplateMethods()
{
    ::fwTools::Object::sptr obj = ::fwTools::Object::New();
    ::TestService::sptr service;

    // Test if the object support the service
    CPPUNIT_ASSERT( ::fwServices::registry::ServiceFactory::getDefault()->support(obj->getClassname(), "::TestService") );

    // Test adding service
    ::fwServices::add< ::TestService >(obj, "::TestServiceImplementation");
    CPPUNIT_ASSERT(::fwServices::OSR::has(obj, "::TestService") );

    // Test getting the service its object
    service = ::fwServices::get< ::TestService >(obj);
    CPPUNIT_ASSERT(service);
    CPPUNIT_ASSERT_EQUAL(obj, service->getObject< ::fwTools::Object >());

    // Test erasing service
    ::fwServices::OSR::unregisterService(service);
    CPPUNIT_ASSERT( ::fwServices::OSR::has(obj, "::TestService") == false );
}

//------------------------------------------------------------------------------

void ServiceTest::testServiceCreationWithUUID()
{
    const std::string myUUID = "myUUID";
    const std::string myUUID2 = "myUUID2";
    const std::string myUUID3 = "myUUID3";
    size_t nbServices = 0;

    ::fwTools::Object::sptr obj = ::fwTools::Object::New();
    ::fwServices::IService::sptr service;

    // Test if the object support the service
    CPPUNIT_ASSERT( ::fwServices::registry::ServiceFactory::getDefault()->support(obj->getClassname(), "::TestService") );

    // Test adding service
    ::fwServices::add(obj, "::TestService", "::TestServiceImplementation", myUUID);
    ::fwServices::add(obj, "::TestService", "::TestServiceImplementation", myUUID2);
    nbServices = 2;
    CPPUNIT_ASSERT(::fwServices::OSR::has(obj, "::TestService") );
    CPPUNIT_ASSERT(::fwTools::fwID::exist(myUUID) );
    CPPUNIT_ASSERT(::fwTools::fwID::exist(myUUID2));

    // Test getting the service its object
    service = ::fwServices::get< ::TestService >(obj, myUUID2);
    CPPUNIT_ASSERT(service);
    CPPUNIT_ASSERT_EQUAL(obj, service->getObject< ::fwTools::Object >());
    CPPUNIT_ASSERT_EQUAL(myUUID2, service ->getID());
    CPPUNIT_ASSERT( ::fwServices::get(obj, "::TestService", myUUID3) == NULL );
    CPPUNIT_ASSERT_EQUAL( nbServices, ::fwServices::OSR::getServices(obj, "::TestService").size() );

    // Test erasing service
    ::fwServices::OSR::unregisterService(service);
    nbServices--;
    CPPUNIT_ASSERT_EQUAL( nbServices, ::fwServices::OSR::getServices(obj, "::TestService").size() );
}

//------------------------------------------------------------------------------

void ServiceTest::testStartStopUpdate()
{
    const std::string myUUID = "myUUID";

    ::fwTools::Object::sptr obj = ::fwTools::Object::New();
    ::TestService::sptr service;

    // Add service
    CPPUNIT_ASSERT( ::fwServices::registry::ServiceFactory::getDefault()->support(obj->getClassname(), "::TestService") );
    ::fwServices::add(obj, "::TestService", "::TestServiceImplementation", myUUID);
    CPPUNIT_ASSERT(::fwServices::OSR::has(obj, "::TestService") );

    // Get service
    service = ::fwServices::get< ::TestService >(obj, myUUID);
    CPPUNIT_ASSERT(service);

    // Service must be stop when it is created
    CPPUNIT_ASSERT(service->isStopped());

    // Start service
    service->start();
    CPPUNIT_ASSERT(service->isStarted());

    // Update service
    service->update();
    CPPUNIT_ASSERT(service->getIsUpdated());

    // Stop service
    service->stop();
    CPPUNIT_ASSERT(service->isStopped());

    // Erase Service
    ::fwServices::OSR::unregisterService(service);
    CPPUNIT_ASSERT(::fwServices::OSR::has(obj, "::TestService") == false );
}

//------------------------------------------------------------------------------

void ServiceTest::testCommunication()
{
    const std::string EVENT = "EVENT";
    const std::string service1UUID = "service1UUID";
    const std::string service2UUID = "service2UUID";

    ::fwData::Composite::NewSptr obj;
    ::TestService::sptr service1;
    ::TestService::sptr service2;

    // Add services
    CPPUNIT_ASSERT( ::fwServices::registry::ServiceFactory::getDefault()->support(obj->getClassname(), "::TestService") );
    ::fwServices::add(obj, "::TestService", "::TestServiceImplementation", service1UUID);
    CPPUNIT_ASSERT(::fwServices::OSR::has(obj, "::TestService") );
    service1 = ::fwServices::get< ::TestService >(obj, service1UUID);
    CPPUNIT_ASSERT(service1);

    ::fwServices::add(obj, "::TestService", "::TestServiceImplementation", service2UUID);
    service2 = ::fwServices::get< ::TestService >(obj, service2UUID);
    CPPUNIT_ASSERT(service2);

    // Start services
    service1->start();
    service2->start();
    CPPUNIT_ASSERT(service1->isStarted());
    CPPUNIT_ASSERT(service2->isStarted());

    // Create message
    ::fwServices::ObjectMsg::NewSptr objMsg;
    objMsg->addEvent(EVENT);
    CPPUNIT_ASSERT(objMsg->hasEvent(EVENT));

    // Register communication channel
    ::fwServices::registerCommunicationChannel(obj, service1)->start();
    ::fwServices::registerCommunicationChannel(obj, service2)->start();

    // Service1 send notification
    ::fwServices::IEditionService::notify(service1, obj, objMsg);

    // Test if service2 has received the message
    CPPUNIT_ASSERT(service2->getIsUpdatedMessage());
    service1->stop();
}

//------------------------------------------------------------------------------

void ServiceTest::testObjectCreationWithConfig()
{
    const std::string objectUUID = "objectUUID";
    const std::string serviceUUID1 = "myTestService1";
    const std::string serviceUUID2 = "myTestService2";

    // Create object configuration
    ::fwRuntime::ConfigurationElement::sptr config = buildObjectConfig() ;

    // Create the object and its services from the configuration
    ::fwServices::AppConfigManager::NewSptr configManager;
    configManager->setConfig( config );
    configManager->create();
    ::fwTools::Object::sptr obj = configManager->getConfigRoot();

    // Test object uid
    CPPUNIT_ASSERT_EQUAL(objectUUID, obj->getID());

    // Test if object's service is created
    CPPUNIT_ASSERT( ::fwServices::OSR::has(obj, "::TestService"));

    // Test start services
    configManager->start();
    CPPUNIT_ASSERT( ::fwServices::get< ::TestService >(obj, serviceUUID1)->isStarted() );
    CPPUNIT_ASSERT( ::fwServices::get< ::TestService >(obj, serviceUUID2)->isStarted() );

    // Test update services
    configManager->update();
    CPPUNIT_ASSERT( ::fwServices::get< ::TestService >(obj, serviceUUID1)->getIsUpdated() );
    CPPUNIT_ASSERT( ::fwServices::get< ::TestService >(obj, serviceUUID2)->getIsUpdated() == false );

    // Test stop services
    configManager->stop();
    CPPUNIT_ASSERT( ::fwServices::get< ::TestService >(obj, serviceUUID1)->isStopped() );
    CPPUNIT_ASSERT( ::fwServices::get< ::TestService >(obj, serviceUUID2)->isStopped() );

    configManager->destroy();
}

//------------------------------------------------------------------------------

::boost::shared_ptr< ::fwRuntime::EConfigurationElement > ServiceTest::buildServiceConfig()
{
    ::boost::shared_ptr< ::fwRuntime::EConfigurationElement > cfg ( new ::fwRuntime::EConfigurationElement("service")) ;
    cfg->setAttributeValue( "uid" , "myTestService" ) ;
    cfg->setAttributeValue( "type" , "::TestService" ) ;
    cfg->setAttributeValue( "implementation" , "::TestServiceImplementation" ) ;
    cfg->setAttributeValue( "autoComChannel" , "no" ) ;

    return cfg ;
}

//------------------------------------------------------------------------------

::boost::shared_ptr< ::fwRuntime::ConfigurationElement > ServiceTest::buildObjectConfig()
{
    // Configuration on fwTools::Object which uid is objectUUID
    ::boost::shared_ptr< ::fwRuntime::EConfigurationElement > cfg ( new ::fwRuntime::EConfigurationElement("object")) ;
    cfg->setAttributeValue( "uid" , "objectUUID") ;
    cfg->setAttributeValue( "type" , "::fwData::Composite") ;

    // Object's service A
    ::boost::shared_ptr< ::fwRuntime::EConfigurationElement > serviceA = cfg->addConfigurationElement("service");
    serviceA->setAttributeValue( "uid" , "myTestService1" ) ;
    serviceA->setAttributeValue( "type" , "::TestService" ) ;
    serviceA->setAttributeValue( "implementation" , "::TestServiceImplementation" ) ;
    serviceA->setAttributeValue( "autoComChannel" , "no" ) ;

    // Object's service B
    ::boost::shared_ptr< ::fwRuntime::EConfigurationElement > serviceB = cfg->addConfigurationElement("service");
    serviceB->setAttributeValue( "uid" , "myTestService2" ) ;
    serviceB->setAttributeValue( "type" , "::TestService" ) ;
    serviceB->setAttributeValue( "implementation" , "::TestServiceImplementation" ) ;
    serviceB->setAttributeValue( "autoComChannel" , "no" ) ;

    // Start method from object's services
    ::boost::shared_ptr< ::fwRuntime::EConfigurationElement > startA = cfg->addConfigurationElement("start");
    startA->setAttributeValue( "uid" , "myTestService1" ) ;
    ::boost::shared_ptr< ::fwRuntime::EConfigurationElement > startB = cfg->addConfigurationElement("start");
    startB->setAttributeValue( "uid" , "myTestService2" ) ;

    // Update method from object's services
    ::boost::shared_ptr< ::fwRuntime::EConfigurationElement > updateA = cfg->addConfigurationElement("update");
    updateA->setAttributeValue( "uid" , "myTestService1" ) ;

    // Stop method from object's services
    ::boost::shared_ptr< ::fwRuntime::EConfigurationElement > stopA = cfg->addConfigurationElement("stop");
    stopA->setAttributeValue( "uid" , "myTestService1" ) ;
    ::boost::shared_ptr< ::fwRuntime::EConfigurationElement > stopB = cfg->addConfigurationElement("stop");
    stopB->setAttributeValue( "uid" , "myTestService2" ) ;

    return cfg ;
}

//------------------------------------------------------------------------------
