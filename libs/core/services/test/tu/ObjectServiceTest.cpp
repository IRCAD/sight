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

#include "ObjectServiceTest.hpp"

#include "TestService.hpp"

#include <core/Profiling.hpp>
#include <core/thread/Worker.hpp>

#include <data/Float.hpp>
#include <data/Integer.hpp>

#include <services/IService.hpp>
#include <services/macros.hpp>
#include <services/op/Add.hpp>
#include <services/op/Get.hpp>
#include <services/registry/ServiceFactory.hpp>

#include <unordered_set>

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION( ::sight::services::ut::ObjectServiceTest );

//------------------------------------------------------------------------------

namespace sight::services
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

void ObjectServiceTest::registerKeyTest()
{
    const std::string srvType("::sight::services::ut::TestService");
    const std::string srvImplementation1("::sight::services::ut::TestServiceImplementation");
    const std::string srvImplementation2("::sight::services::ut::TestServiceImplementation2");

    data::Integer::sptr obj1 = data::Integer::New();
    data::Integer::sptr obj2 = data::Integer::New();
    data::Integer::sptr obj3 = data::Integer::New();

    auto service1 = services::registry::ServiceFactory::getDefault()->create( srvType, srvImplementation1 );
    auto service2 = services::registry::ServiceFactory::getDefault()->create( srvType, srvImplementation2 );
    auto service3 = services::registry::ServiceFactory::getDefault()->create( srvType, srvImplementation1 );

    services::registry::ObjectService osr;
    CPPUNIT_ASSERT_EQUAL(false, service1->hasObjectId("key1"));
    service1->registerObject("uid1", "key1", services::IService::AccessType::INOUT);
    service1->registerObject("uid2", "key2", services::IService::AccessType::INOUT);
    service1->registerObject("uid3", "key3", services::IService::AccessType::INOUT);

    CPPUNIT_ASSERT_EQUAL(true, service1->hasObjectId("key1"));
    CPPUNIT_ASSERT_EQUAL(true, service1->hasObjectId("key2"));
    CPPUNIT_ASSERT_EQUAL(true, service1->hasObjectId("key3"));
    CPPUNIT_ASSERT_EQUAL(false, service1->hasObjectId("another_key"));

    CPPUNIT_ASSERT_EQUAL(std::string("uid1"), service1->getObjectId("key1"));
    CPPUNIT_ASSERT_EQUAL(std::string("uid2"), service1->getObjectId("key2"));
    CPPUNIT_ASSERT_EQUAL(std::string("uid3"), service1->getObjectId("key3"));
    CPPUNIT_ASSERT_THROW(service1->getObjectId("another_key"), core::Exception);

    osr.registerService(obj1, "key1", services::IService::AccessType::INOUT, service1);
    osr.registerService(obj2, "key2", services::IService::AccessType::INOUT, service1);
    osr.registerService(obj3, "key3", services::IService::AccessType::INOUT, service1);

    CPPUNIT_ASSERT( osr.isRegistered("key1", services::IService::AccessType::INOUT, service1) );
    CPPUNIT_ASSERT( osr.isRegistered("key2", services::IService::AccessType::INOUT, service1) );
    CPPUNIT_ASSERT( osr.isRegistered("key3", services::IService::AccessType::INOUT, service1) );
    CPPUNIT_ASSERT( obj1 == osr.getRegistered("key1", services::IService::AccessType::INOUT, service1) );
    CPPUNIT_ASSERT( obj2 == osr.getRegistered("key2", services::IService::AccessType::INOUT, service1) );
    CPPUNIT_ASSERT( obj3 == osr.getRegistered("key3", services::IService::AccessType::INOUT, service1) );

    osr.registerService(obj1, "key1", services::IService::AccessType::INOUT, service2);
    osr.registerService(obj2, "key2", services::IService::AccessType::INOUT, service2);

    CPPUNIT_ASSERT( osr.isRegistered("key1", services::IService::AccessType::INOUT, service2) );
    CPPUNIT_ASSERT( osr.isRegistered("key2", services::IService::AccessType::INOUT, service2) );
    CPPUNIT_ASSERT( obj1 == osr.getRegistered("key1", services::IService::AccessType::INOUT, service2) );
    CPPUNIT_ASSERT( obj2 == osr.getRegistered("key2", services::IService::AccessType::INOUT, service2) );

    osr.registerService(obj3, "key3", services::IService::AccessType::INOUT, service3);
    CPPUNIT_ASSERT( osr.isRegistered("key3", services::IService::AccessType::INOUT, service3) );
    CPPUNIT_ASSERT( obj3 == osr.getRegistered("key3", services::IService::AccessType::INOUT, service3) );

    // 3 services in total
    {
        auto servicesByType         = osr.getServices( srvType );
        auto servicesByTemplateType = osr.getServices< services::ut::TestService >( );

        CPPUNIT_ASSERT_EQUAL(size_t(3), servicesByType.size());
        CPPUNIT_ASSERT(std::equal(servicesByType.begin(), servicesByType.end(), servicesByTemplateType.begin()));
    }

    // 2 services of type "::sight::services::ut::TestServiceImplementation"
    {
        auto servicesByType         = osr.getServices( srvImplementation1 );
        auto servicesByTemplateType = osr.getServices< services::ut::TestServiceImplementation >( );

        CPPUNIT_ASSERT_EQUAL(size_t(2), servicesByType.size());
        CPPUNIT_ASSERT(std::equal(servicesByType.begin(), servicesByType.end(), servicesByTemplateType.begin()));
    }

    // 1 service of type "::sight::services::ut::TestServiceImplementation2"
    {
        auto servicesByType         = osr.getServices( srvImplementation2 );
        auto servicesByTemplateType = osr.getServices< services::ut::TestServiceImplementation2 >( );

        CPPUNIT_ASSERT_EQUAL(size_t(1), servicesByType.size());
        CPPUNIT_ASSERT(std::equal(servicesByType.begin(), servicesByType.end(), servicesByTemplateType.begin()));
    }

    auto servicesByType = osr.getServices( srvType );
    CPPUNIT_ASSERT_EQUAL(size_t(3), servicesByType.size());

    CPPUNIT_ASSERT(servicesByType.find(service1) != servicesByType.end());
    CPPUNIT_ASSERT(servicesByType.find(service2) != servicesByType.end());
    CPPUNIT_ASSERT(servicesByType.find(service3) != servicesByType.end());

    // Remove key 1 from service 1 and check consistency
    osr.unregisterService("key1", services::IService::AccessType::INOUT, service1);
    CPPUNIT_ASSERT( false == osr.isRegistered("key1", services::IService::AccessType::INOUT, service1) );
    servicesByType = osr.getServices( srvType );
    CPPUNIT_ASSERT_EQUAL(size_t(3), servicesByType.size());

    osr.unregisterService(service1);
    servicesByType = osr.getServices( srvType );
    CPPUNIT_ASSERT_EQUAL(size_t(2), servicesByType.size());

    // Remove key 2 from service 2 and check consistency
    osr.unregisterService("key2", services::IService::AccessType::INOUT, service2);
    CPPUNIT_ASSERT( false == osr.isRegistered("key2", services::IService::AccessType::INOUT, service2) );

    servicesByType = osr.getServices( srvType );
    CPPUNIT_ASSERT_EQUAL(size_t(2), servicesByType.size());

    // Register key 2 to service 1 just for fun
    osr.registerService(obj2, "key2", services::IService::AccessType::INOUT, service1);
    CPPUNIT_ASSERT( osr.isRegistered("key2", services::IService::AccessType::INOUT, service1) );
    CPPUNIT_ASSERT( obj2 == osr.getRegistered("key2", services::IService::AccessType::INOUT, service1) );
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
    services::registry::ObjectService osr;

    const std::string srvType("::sight::services::ut::TestService");
    const std::string srvImplementation1("::sight::services::ut::TestServiceImplementation");

    data::Integer::sptr obj1 = data::Integer::New();
    data::Integer::sptr obj2 = data::Integer::New();

    auto service1 = services::registry::ServiceFactory::getDefault()->create( srvType, srvImplementation1 );

    auto worker         = core::thread::Worker::New();
    auto slotRegister   = core::com::newSlot( &ObjectServiceTest::registerService, this);
    auto slotUnregister = core::com::newSlot( &ObjectServiceTest::unregisterService, this);
    slotRegister->setWorker(worker);
    slotUnregister->setWorker(worker);

    osr.signal(services::registry::ObjectService::s_REGISTERED_SIG)->connect(slotRegister);
    osr.signal(services::registry::ObjectService::s_UNREGISTERED_SIG)->connect(slotUnregister);

    // Register callback test
    // Each time we wait the slot with a timeout to avoid blocking the test in case of failure
    service1->registerObject("uid1", "key1", services::IService::AccessType::OUTPUT);
    osr.registerServiceOutput(obj1, "key1", service1);
    {
        std::unique_lock<std::mutex> lock(m_mutex);
        m_condition.wait_for(lock, std::chrono::milliseconds(1000), [this] { return m_ret == "uid1"; });

        CPPUNIT_ASSERT_EQUAL(std::string("uid1"), m_ret);
        CPPUNIT_ASSERT(obj1 == m_obj);
    }

    service1->registerObject("uid2", "key2", services::IService::AccessType::OUTPUT);
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

void ObjectServiceTest::registerService(data::Object::sptr obj, const std::string& id)
{
    {
        std::unique_lock<std::mutex> lock(m_mutex);
        m_obj = obj;
        m_ret = id;
    }
    m_condition.notify_one();

}

//------------------------------------------------------------------------------

void ObjectServiceTest::unregisterService(data::Object::sptr obj, const std::string& id)
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
} //namespace sight::services
