/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2018.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "ObjectServiceTest.hpp"

#include "TestService.hpp"

#include <fwServices/IService.hpp>
#include <fwServices/macros.hpp>
#include <fwServices/op/Add.hpp>
#include <fwServices/op/Get.hpp>
#include <fwServices/registry/ServiceFactory.hpp>

#include <fwCore/Profiling.hpp>

#include <fwData/Float.hpp>
#include <fwData/Integer.hpp>

#include <fwThread/Worker.hpp>

#include <unordered_set>

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION( ::fwServices::ut::ObjectServiceTest );

//------------------------------------------------------------------------------

namespace fwServices
{
namespace ut
{

//------------------------------------------------------------------------------

void ObjectServiceTest::setUp()
{
    // Set up context before running a test.
}

//------------------------------------------------------------------------------

void ObjectServiceTest::tearDown()
{
    // Clean up after the test run.
}

//------------------------------------------------------------------------------

void ObjectServiceTest::swapTest()
{
#ifndef REMOVE_DEPRECATED
    FW_DEPRECATED_MSG("This test check a deprecated method.", "20.0");
    const std::string srvType("::fwServices::ut::TestService");
    const std::string srvImplementation("::fwServices::ut::TestServiceImplementation");

    ::fwData::Integer::sptr objSrv1 = ::fwData::Integer::New();
    ::fwData::Float::sptr objSrv2   = ::fwData::Float::New();

    ::fwServices::IService::sptr service;
    service = ::fwServices::registry::ServiceFactory::getDefault()->create( srvType, srvImplementation );

    ::fwServices::registry::ObjectService osr;

    osr.registerService(objSrv1, service);

    auto servicesByTemplateTypeAndObj1 = osr.getServices< ::fwServices::ut::TestService >( objSrv1 );
    auto servicesByTemplateTypeAndObj2 = osr.getServices< ::fwServices::ut::TestService >( objSrv2 );

    CPPUNIT_ASSERT_EQUAL(size_t(1), servicesByTemplateTypeAndObj1.size());
    CPPUNIT_ASSERT_EQUAL(size_t(0), servicesByTemplateTypeAndObj2.size());
    ::fwServices::IService::sptr osrSrv = *servicesByTemplateTypeAndObj1.begin();
    CPPUNIT_ASSERT_EQUAL(service, osrSrv);

    osr.swapService(objSrv2, service);

    CPPUNIT_ASSERT(objSrv2 == service->getObject());

    servicesByTemplateTypeAndObj1 = osr.getServices< ::fwServices::ut::TestService >( objSrv1 );
    servicesByTemplateTypeAndObj2 = osr.getServices< ::fwServices::ut::TestService >( objSrv2 );

    CPPUNIT_ASSERT_EQUAL(size_t(0), servicesByTemplateTypeAndObj1.size());
    CPPUNIT_ASSERT_EQUAL(size_t(1), servicesByTemplateTypeAndObj2.size());
    osrSrv = *servicesByTemplateTypeAndObj2.begin();
    CPPUNIT_ASSERT_EQUAL(service, osrSrv);

    osr.unregisterService(service);
#endif
}

//------------------------------------------------------------------------------

void ObjectServiceTest::registerTest()
{
#ifndef REMOVE_DEPRECATED
    FW_DEPRECATED_MSG("This test check a deprecated method.", "20.0");

    const std::string srvType("::fwServices::ut::TestService");
    const std::string srvImplementation("::fwServices::ut::TestServiceImplementation");

    ::fwData::Integer::sptr obj = ::fwData::Integer::New();

    ::fwServices::IService::sptr service;
    service = ::fwServices::registry::ServiceFactory::getDefault()->create( srvType, srvImplementation );

    ::fwServices::registry::ObjectService osr;

    osr.registerService(obj, service);
    CPPUNIT_ASSERT(osr.has(obj, srvType) );

    typedef ::fwServices::registry::ObjectService::ServiceVectorType ServiceVectorType;
    ServiceVectorType servicesByType       = osr.getServices( srvType );
    ServiceVectorType servicesByObjAndType = osr.getServices( obj, srvType );
    auto servicesByTemplateType            = osr.getServices< ::fwServices::ut::TestService >( );
    auto servicesByTemplateTypeAndObj      = osr.getServices< ::fwServices::ut::TestService >( obj );

    CPPUNIT_ASSERT(servicesByType == servicesByObjAndType);
    CPPUNIT_ASSERT(std::equal(servicesByType.begin(), servicesByType.end(), servicesByTemplateType.begin()));
    CPPUNIT_ASSERT(std::equal(servicesByType.begin(), servicesByType.end(), servicesByTemplateTypeAndObj.begin()));

    CPPUNIT_ASSERT_EQUAL(size_t(1), servicesByType.size());
    typedef ::fwServices::registry::ObjectService::ObjectVectorType ObjectVectorType;
    ::fwServices::IService::sptr osrService = *servicesByType.begin();
    CPPUNIT_ASSERT_EQUAL(service, osrService);

    ObjectVectorType objects = osr.getObjects();
    CPPUNIT_ASSERT_EQUAL(size_t(1), objects.size());
    ::fwData::Object::csptr osrObject = *objects.begin();
    CPPUNIT_ASSERT(obj == osrObject);

    osr.unregisterService(service);
    CPPUNIT_ASSERT( osr.has(obj, srvType) == false );
    servicesByType = osr.getServices( srvType );
    CPPUNIT_ASSERT( servicesByType.empty() );
#endif
}

//------------------------------------------------------------------------------

void ObjectServiceTest::registerKeyTest()
{
    const std::string srvType("::fwServices::ut::TestService");
    const std::string srvImplementation1("::fwServices::ut::TestServiceImplementation");
    const std::string srvImplementation2("::fwServices::ut::TestServiceImplementation2");

    ::fwData::Integer::sptr obj1 = ::fwData::Integer::New();
    ::fwData::Integer::sptr obj2 = ::fwData::Integer::New();
    ::fwData::Integer::sptr obj3 = ::fwData::Integer::New();

    auto service1 = ::fwServices::registry::ServiceFactory::getDefault()->create( srvType, srvImplementation1 );
    auto service2 = ::fwServices::registry::ServiceFactory::getDefault()->create( srvType, srvImplementation2 );
    auto service3 = ::fwServices::registry::ServiceFactory::getDefault()->create( srvType, srvImplementation1 );

    ::fwServices::registry::ObjectService osr;

    service1->setObjectId("key1", "uid1");
    service1->setObjectId("key2", "uid2");
    service1->setObjectId("key3", "uid3");

    CPPUNIT_ASSERT_EQUAL(true, service1->hasObjectId("key1"));
    CPPUNIT_ASSERT_EQUAL(true, service1->hasObjectId("key2"));
    CPPUNIT_ASSERT_EQUAL(true, service1->hasObjectId("key3"));
    CPPUNIT_ASSERT_EQUAL(false, service1->hasObjectId("another_key"));

    CPPUNIT_ASSERT_EQUAL(std::string("uid1"), service1->getObjectId("key1"));
    CPPUNIT_ASSERT_EQUAL(std::string("uid2"), service1->getObjectId("key2"));
    CPPUNIT_ASSERT_EQUAL(std::string("uid3"), service1->getObjectId("key3"));
    CPPUNIT_ASSERT_THROW(service1->getObjectId("another_key"), ::fwCore::Exception);

    service2->setObjectId("key1", "uid1");
    service2->setObjectId("key2", "uid2");

    service3->setObjectId("key3", "uid3");

    osr.registerService(obj1, "key1", ::fwServices::IService::AccessType::INOUT, service1);
    osr.registerService(obj2, "key2", ::fwServices::IService::AccessType::INOUT, service1);
    osr.registerService(obj3, "key3", ::fwServices::IService::AccessType::INOUT, service1);

    CPPUNIT_ASSERT( osr.isRegistered("key1", ::fwServices::IService::AccessType::INOUT, service1) );
    CPPUNIT_ASSERT( osr.isRegistered("key2", ::fwServices::IService::AccessType::INOUT, service1) );
    CPPUNIT_ASSERT( osr.isRegistered("key3", ::fwServices::IService::AccessType::INOUT, service1) );
    CPPUNIT_ASSERT( obj1 == osr.getRegistered("key1", ::fwServices::IService::AccessType::INOUT, service1) );
    CPPUNIT_ASSERT( obj2 == osr.getRegistered("key2", ::fwServices::IService::AccessType::INOUT, service1) );
    CPPUNIT_ASSERT( obj3 == osr.getRegistered("key3", ::fwServices::IService::AccessType::INOUT, service1) );

    osr.registerService(obj1, "key1", ::fwServices::IService::AccessType::INOUT, service2);
    osr.registerService(obj2, "key2", ::fwServices::IService::AccessType::INOUT, service2);

    CPPUNIT_ASSERT( osr.isRegistered("key1", ::fwServices::IService::AccessType::INOUT, service2) );
    CPPUNIT_ASSERT( osr.isRegistered("key2", ::fwServices::IService::AccessType::INOUT, service2) );
    CPPUNIT_ASSERT( obj1 == osr.getRegistered("key1", ::fwServices::IService::AccessType::INOUT, service2) );
    CPPUNIT_ASSERT( obj2 == osr.getRegistered("key2", ::fwServices::IService::AccessType::INOUT, service2) );

    osr.registerService(obj3, "key3", ::fwServices::IService::AccessType::INOUT, service3);
    CPPUNIT_ASSERT( osr.isRegistered("key3", ::fwServices::IService::AccessType::INOUT, service3) );
    CPPUNIT_ASSERT( obj3 == osr.getRegistered("key3", ::fwServices::IService::AccessType::INOUT, service3) );

    // 3 services in total
    {
        auto servicesByType         = osr.getServices( srvType );
        auto servicesByTemplateType = osr.getServices< ::fwServices::ut::TestService >( );

        CPPUNIT_ASSERT_EQUAL(size_t(3), servicesByType.size());
        CPPUNIT_ASSERT(std::equal(servicesByType.begin(), servicesByType.end(), servicesByTemplateType.begin()));
    }

    // 2 services of type "::fwServices::ut::TestServiceImplementation"
    {
        auto servicesByType         = osr.getServices( srvImplementation1 );
        auto servicesByTemplateType = osr.getServices< ::fwServices::ut::TestServiceImplementation >( );

        CPPUNIT_ASSERT_EQUAL(size_t(2), servicesByType.size());
        CPPUNIT_ASSERT(std::equal(servicesByType.begin(), servicesByType.end(), servicesByTemplateType.begin()));
    }

    // 1 service of type "::fwServices::ut::TestServiceImplementation2"
    {
        auto servicesByType         = osr.getServices( srvImplementation2 );
        auto servicesByTemplateType = osr.getServices< ::fwServices::ut::TestServiceImplementation2 >( );

        CPPUNIT_ASSERT_EQUAL(size_t(1), servicesByType.size());
        CPPUNIT_ASSERT(std::equal(servicesByType.begin(), servicesByType.end(), servicesByTemplateType.begin()));
    }

#ifndef REMOVE_DEPRECATED
    // Begin deprecated methods
    // 2 services of type "::fwServices::ut::TestService" working on obj1
    {
        auto srvByTplTypeAndObj1 = osr.getServices< ::fwServices::ut::TestService >( obj1 );

        CPPUNIT_ASSERT_EQUAL(size_t(2), srvByTplTypeAndObj1.size());
    }

    // 1 service of type "::fwServices::ut::TestServiceImplementation" working on obj1
    {
        auto srvByTplTypeAndObj1 = osr.getServices< ::fwServices::ut::TestServiceImplementation >( obj1 );
        CPPUNIT_ASSERT_EQUAL(size_t(1), srvByTplTypeAndObj1.size());
        CPPUNIT_ASSERT(service1 == *srvByTplTypeAndObj1.begin());
    }

    // 1 service of type "::fwServices::ut::TestServiceImplementation2" working on obj1
    {
        auto srvByTplTypeAndObj1 = osr.getServices< ::fwServices::ut::TestServiceImplementation2 >( obj1 );
        CPPUNIT_ASSERT_EQUAL(size_t(1), srvByTplTypeAndObj1.size());
        CPPUNIT_ASSERT(service2 == *srvByTplTypeAndObj1.begin());
    }

    // 1 service of type "::fwServices::ut::TestServiceImplementation2" working on obj2
    {
        auto srvByTplTypeAndObj2 = osr.getServices< ::fwServices::ut::TestServiceImplementation2 >( obj2 );
        CPPUNIT_ASSERT_EQUAL(size_t(1), srvByTplTypeAndObj2.size());
        CPPUNIT_ASSERT(service2 == *srvByTplTypeAndObj2.begin());
    }

    // 2 services of type "::fwServices::ut::TestServiceImplementation" working on obj3
    {
        auto srvByObjAndType3    = osr.getServices( obj3, srvType );
        auto srvByTplTypeAndObj3 = osr.getServices< ::fwServices::ut::TestServiceImplementation >( obj3 );

        CPPUNIT_ASSERT_EQUAL(size_t(2), srvByObjAndType3.size());
        CPPUNIT_ASSERT(std::equal(srvByObjAndType3.begin(), srvByObjAndType3.end(), srvByTplTypeAndObj3.begin()));
    }

    // End deprecated methods
#endif

    auto servicesByType = osr.getServices( srvType );
    CPPUNIT_ASSERT_EQUAL(size_t(3), servicesByType.size());

    CPPUNIT_ASSERT(servicesByType.find(service1) != servicesByType.end());
    CPPUNIT_ASSERT(servicesByType.find(service2) != servicesByType.end());
    CPPUNIT_ASSERT(servicesByType.find(service3) != servicesByType.end());

#ifndef REMOVE_DEPRECATED
    typedef ::fwServices::registry::ObjectService::ObjectVectorType ObjectVectorType;
    ObjectVectorType objects = osr.getObjects();
    CPPUNIT_ASSERT_EQUAL(size_t(3), objects.size());

    CPPUNIT_ASSERT(objects.find(obj1) != objects.end());
    CPPUNIT_ASSERT(objects.find(obj2) != objects.end());
    CPPUNIT_ASSERT(objects.find(obj3) != objects.end());
#endif

    // Remove key 1 from service 1 and check consistency
    osr.unregisterService("key1", ::fwServices::IService::AccessType::INOUT, service1);
    CPPUNIT_ASSERT( false == osr.isRegistered("key1", ::fwServices::IService::AccessType::INOUT, service1) );
    servicesByType = osr.getServices( srvType );
    CPPUNIT_ASSERT_EQUAL(size_t(3), servicesByType.size());

    osr.unregisterService(service1);
    servicesByType = osr.getServices( srvType );
    CPPUNIT_ASSERT_EQUAL(size_t(2), servicesByType.size());

    // Remove key 2 from service 2 and check consistency
    osr.unregisterService("key2", ::fwServices::IService::AccessType::INOUT, service2);
    CPPUNIT_ASSERT( false == osr.isRegistered("key2", ::fwServices::IService::AccessType::INOUT, service2) );

    servicesByType = osr.getServices( srvType );
    CPPUNIT_ASSERT_EQUAL(size_t(2), servicesByType.size());

    // Register key 2 to service 1 just for fun
    osr.registerService(obj2, "key2", ::fwServices::IService::AccessType::INOUT, service1);
    CPPUNIT_ASSERT( osr.isRegistered("key2", ::fwServices::IService::AccessType::INOUT, service1) );
    CPPUNIT_ASSERT( obj2 == osr.getRegistered("key2", ::fwServices::IService::AccessType::INOUT, service1) );
    servicesByType = osr.getServices( srvType );
    CPPUNIT_ASSERT_EQUAL(size_t(3), servicesByType.size());

    // Remove service 3 and check consistency
    osr.unregisterService(service3);
    servicesByType = osr.getServices( srvType );
    CPPUNIT_ASSERT_EQUAL(size_t(2), servicesByType.size());

    osr.unregisterService(service2);

    servicesByType = osr.getServices( srvType );
    CPPUNIT_ASSERT_EQUAL(size_t(1), servicesByType.size());

    osr.unregisterService(service1);
    servicesByType = osr.getServices( srvType );
    CPPUNIT_ASSERT( servicesByType.empty() );
}

//------------------------------------------------------------------------------

void ObjectServiceTest::registerConnectionTest()
{
    ::fwServices::registry::ObjectService osr;

    const std::string srvType("::fwServices::ut::TestService");
    const std::string srvImplementation1("::fwServices::ut::TestServiceImplementation");

    ::fwData::Integer::sptr obj1 = ::fwData::Integer::New();
    ::fwData::Integer::sptr obj2 = ::fwData::Integer::New();

    auto service1 = ::fwServices::registry::ServiceFactory::getDefault()->create( srvType, srvImplementation1 );

    auto worker         = ::fwThread::Worker::New();
    auto slotRegister   = ::fwCom::newSlot( &ObjectServiceTest::registerService, this);
    auto slotUnregister = ::fwCom::newSlot( &ObjectServiceTest::unregisterService, this);
    slotRegister->setWorker(worker);
    slotUnregister->setWorker(worker);

    osr.signal(::fwServices::registry::ObjectService::s_REGISTERED_SIG)->connect(slotRegister);
    osr.signal(::fwServices::registry::ObjectService::s_UNREGISTERED_SIG)->connect(slotUnregister);

    // Register callback test
    // Each time we wait the slot with a timeout to avoid blocking the test in case of failure
    service1->setObjectId("key1", "uid1");
    osr.registerServiceOutput(obj1, "key1", service1);
    {
        std::unique_lock<std::mutex> lock(m_mutex);
        m_condition.wait_for(lock, std::chrono::milliseconds(1000), [this] { return m_ret == "uid1"; });

        CPPUNIT_ASSERT_EQUAL(std::string("uid1"), m_ret);
        CPPUNIT_ASSERT(obj1 == m_obj);
    }

    service1->setObjectId("key2", "uid2");
    osr.registerServiceOutput(obj2, "key2", service1);
    {
        std::unique_lock<std::mutex> lock(m_mutex);
        m_condition.wait_for(lock, std::chrono::milliseconds(1000), [this] { return m_ret == "uid2"; });

        CPPUNIT_ASSERT_EQUAL(std::string("uid2"), m_ret);
        CPPUNIT_ASSERT(obj2 == m_obj);
    }

    // Unregister callback test
    osr.unregisterServiceOutput("key1", service1);
    {
        std::unique_lock<std::mutex> lock(m_mutex);
        m_condition.wait_for(lock, std::chrono::milliseconds(1000), [this] { return m_ret == "uid1"; });

        CPPUNIT_ASSERT_EQUAL(std::string("uid1"), m_ret);
    }
}

//------------------------------------------------------------------------------

void ObjectServiceTest::registerService(::fwData::Object::sptr obj, const std::string& id)
{
    {
        std::unique_lock<std::mutex> lock(m_mutex);
        m_obj = obj;
        m_ret = id;
    }
    m_condition.notify_one();

}

//------------------------------------------------------------------------------

void ObjectServiceTest::unregisterService(::fwData::Object::sptr obj, const std::string& id)
{
    {
        std::unique_lock<std::mutex> lock(m_mutex);
        m_obj = obj;
        m_ret = id;
    }
    m_condition.notify_one();

}

//------------------------------------------------------------------------------

} //namespace ut
} //namespace fwServices
