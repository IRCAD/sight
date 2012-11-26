/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2012.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <fwData/Object.hpp>
#include <fwTools/fwID.hpp>

#include <fwData/Composite.hpp>
#include <fwData/Image.hpp>
#include <fwData/Integer.hpp>

#include <fwServices/IService.hpp>
#include <fwServices/IEditionService.hpp>
#include <fwServices/Base.hpp>
#include <fwServices/macros.hpp>
#include <fwServices/registry/ServiceFactory.hpp>
#include <fwServices/AppConfigManager.hpp>
#include <fwServices/helper/SigSlotConnection.hpp>
#include <fwServices/registry/ActiveWorkers.hpp>

#include <fwRuntime/EConfigurationElement.hpp>
#include <fwRuntime/helper.hpp>

#include "ServiceTest.hpp"

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION( ::fwServices::ut::ServiceTest );

//------------------------------------------------------------------------------

fwServicesRegisterMacro( ::fwServices::ut::TestService , ::fwServices::ut::TestServiceImplementation , ::fwData::Object ) ;

//------------------------------------------------------------------------------

namespace fwServices
{
namespace ut
{

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
    ::fwData::Integer::sptr obj = ::fwData::Integer::New();
    ::fwServices::IService::sptr service;

    // Test if the object support the service
    CPPUNIT_ASSERT( ::fwServices::registry::ServiceFactory::getDefault()->support(obj->getClassname(), "::fwServices::ut::TestService") );

    // Test adding service
    ::fwServices::add(obj, "::fwServices::ut::TestService", "::fwServices::ut::TestServiceImplementation");
    CPPUNIT_ASSERT(::fwServices::OSR::has(obj, "::fwServices::ut::TestService") );

    // Test getting the service its object
    service = ::fwServices::get(obj, "::fwServices::ut::TestService");
    CPPUNIT_ASSERT(service);
    CPPUNIT_ASSERT_EQUAL(obj, service->getObject< ::fwData::Integer >());

    // Test erasing service
    ::fwServices::OSR::unregisterService(service);
    CPPUNIT_ASSERT( ::fwServices::OSR::has(obj, "::fwServices::ut::TestService") == false );
}

//------------------------------------------------------------------------------

void ServiceTest::testServiceCreationWithTemplateMethods()
{
    ::fwData::Integer::sptr obj = ::fwData::Integer::New();
    ::fwServices::ut::TestService::sptr service;

    // Test if the object support the service
    CPPUNIT_ASSERT( ::fwServices::registry::ServiceFactory::getDefault()->support(obj->getClassname(), "::fwServices::ut::TestService") );

    // Test adding service
    ::fwServices::add< ::fwServices::ut::TestService >(obj, "::fwServices::ut::TestServiceImplementation");
    CPPUNIT_ASSERT(::fwServices::OSR::has(obj, "::fwServices::ut::TestService") );

    // Test getting the service its object
    service = ::fwServices::get< ::fwServices::ut::TestService >(obj);
    CPPUNIT_ASSERT(service);
    CPPUNIT_ASSERT_EQUAL(obj, service->getObject< ::fwData::Integer >());

    // Test erasing service
    ::fwServices::OSR::unregisterService(service);
    CPPUNIT_ASSERT( ::fwServices::OSR::has(obj, "::fwServices::ut::TestService") == false );
}

//------------------------------------------------------------------------------

void ServiceTest::testServiceCreationWithUUID()
{
    const std::string myUUID = "myUUID";
    const std::string myUUID2 = "myUUID2";
    const std::string myUUID3 = "myUUID3";
    size_t nbServices = 0;

    ::fwData::Integer::sptr obj = ::fwData::Integer::New();
    ::fwServices::IService::sptr service;
    ::fwServices::IService::sptr service2;

    // Test if the object support the service
    CPPUNIT_ASSERT( ::fwServices::registry::ServiceFactory::getDefault()->support(obj->getClassname(), "::fwServices::ut::TestService") );

    // Test adding service
    service = ::fwServices::add(obj, "::fwServices::ut::TestService", "::fwServices::ut::TestServiceImplementation", myUUID);
    ::fwServices::add(obj, "::fwServices::ut::TestService", "::fwServices::ut::TestServiceImplementation", myUUID2);
    nbServices = 2;
    CPPUNIT_ASSERT(::fwServices::OSR::has(obj, "::fwServices::ut::TestService") );
    CPPUNIT_ASSERT(::fwTools::fwID::exist(myUUID) );
    CPPUNIT_ASSERT(::fwTools::fwID::exist(myUUID2));

    // Test getting the service its object
    service2 = ::fwServices::get(myUUID2);
    CPPUNIT_ASSERT(service);
    CPPUNIT_ASSERT(service2);
    CPPUNIT_ASSERT_EQUAL(obj, service2->getObject< ::fwData::Integer >());
    CPPUNIT_ASSERT_EQUAL(myUUID2, service2 ->getID());
    CPPUNIT_ASSERT( ::fwTools::fwID::exist(myUUID3) == NULL );
    CPPUNIT_ASSERT_EQUAL( nbServices, ::fwServices::OSR::getServices(obj, "::fwServices::ut::TestService").size() );

    // Test erasing service
    ::fwServices::OSR::unregisterService(service);
    nbServices--;
    CPPUNIT_ASSERT_EQUAL( nbServices, ::fwServices::OSR::getServices(obj, "::fwServices::ut::TestService").size() );
    ::fwServices::OSR::unregisterService(service2);
}

//------------------------------------------------------------------------------

void ServiceTest::testStartStopUpdate()
{
    const std::string myUUID = "myUUID";

    ::fwData::Integer::NewSptr obj;
    ::fwServices::ut::TestService::sptr service;

    // Add service
    CPPUNIT_ASSERT( ::fwServices::registry::ServiceFactory::getDefault()->support(obj->getClassname(), "::fwServices::ut::TestService") );
    ::fwServices::add(obj, "::fwServices::ut::TestService", "::fwServices::ut::TestServiceImplementation", myUUID);
    CPPUNIT_ASSERT(::fwServices::OSR::has(obj, "::fwServices::ut::TestService") );

    // Get service
    service = ::fwServices::ut::TestService::dynamicCast( ::fwServices::get(myUUID) );
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
    CPPUNIT_ASSERT(::fwServices::OSR::has(obj, "::fwServices::ut::TestService") == false );
}

//------------------------------------------------------------------------------

void ServiceTest::testCommunication()
{
    registry::ActiveWorkers::sptr activeWorkers = registry::ActiveWorkers::getDefault();
    activeWorkers->initRegistry();

    const std::string EVENT = "EVENT";
    const std::string service1UUID = "service1UUID";
    const std::string service2UUID = "service2UUID";

    ::fwData::Composite::NewSptr obj;
    ::fwServices::ut::TestService::sptr service1;
    ::fwServices::ut::TestService::sptr service2;

    // Add services
    CPPUNIT_ASSERT( ::fwServices::registry::ServiceFactory::getDefault()->support(obj->getClassname(), "::fwServices::ut::TestService") );
    ::fwServices::add(obj, "::fwServices::ut::TestService", "::fwServices::ut::TestServiceImplementation", service1UUID);
    CPPUNIT_ASSERT(::fwServices::OSR::has(obj, "::fwServices::ut::TestService") );
    service1 = ::fwServices::ut::TestService::dynamicCast( ::fwServices::get(service1UUID) );
    CPPUNIT_ASSERT(service1);

    ::fwServices::add(obj, "::fwServices::ut::TestService", "::fwServices::ut::TestServiceImplementation", service2UUID);
    service2 = ::fwServices::ut::TestService::dynamicCast( ::fwServices::get(service2UUID) );
    CPPUNIT_ASSERT(service2);

    // Start services
    service1->start().wait();
    service2->start().wait();
    CPPUNIT_ASSERT(service1->isStarted());
    CPPUNIT_ASSERT(service2->isStarted());

    // Create message
    ::fwServices::ObjectMsg::NewSptr objMsg;
    objMsg->addEvent(EVENT);
    CPPUNIT_ASSERT(objMsg->hasEvent(EVENT));

    // Register communication channel
    ::fwServices::helper::SigSlotConnection::NewSptr comHelper;
    comHelper->connect( obj, service1, service1->getObjSrvConnections() );
    comHelper->connect( obj, service2, service2->getObjSrvConnections() );

    // Service1 send notification
    ::fwServices::IEditionService::notify(service1, obj, objMsg);

    // Test if service2 has received the message
    service1->stop().wait();
    service2->stop().wait();
    CPPUNIT_ASSERT(service2->getIsUpdatedMessage());

    comHelper->disconnect();
    comHelper.reset();

    ::fwServices::OSR::unregisterService(service1);
    ::fwServices::OSR::unregisterService(service2);

    activeWorkers->clearRegistry();
}

//------------------------------------------------------------------------------

::boost::shared_ptr< ::fwRuntime::EConfigurationElement > ServiceTest::buildServiceConfig()
{
    ::boost::shared_ptr< ::fwRuntime::EConfigurationElement > cfg ( new ::fwRuntime::EConfigurationElement("service")) ;
    cfg->setAttributeValue( "uid" , "myTestService" ) ;
    cfg->setAttributeValue( "type" , "::fwServices::ut::TestService" ) ;
    cfg->setAttributeValue( "implementation" , "::fwServices::ut::TestServiceImplementation" ) ;
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
    serviceA->setAttributeValue( "type" , "::fwServices::ut::TestService" ) ;
    serviceA->setAttributeValue( "implementation" , "::fwServices::ut::TestServiceImplementation" ) ;
    serviceA->setAttributeValue( "autoComChannel" , "no" ) ;

    // Object's service B
    ::boost::shared_ptr< ::fwRuntime::EConfigurationElement > serviceB = cfg->addConfigurationElement("service");
    serviceB->setAttributeValue( "uid" , "myTestService2" ) ;
    serviceB->setAttributeValue( "type" , "::fwServices::ut::TestService" ) ;
    serviceB->setAttributeValue( "implementation" , "::fwServices::ut::TestServiceImplementation" ) ;
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

} //namespace ut
} //namespace fwServices
