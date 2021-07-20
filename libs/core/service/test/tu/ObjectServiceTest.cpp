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

#include <service/base.hpp>
#include <service/extension/Factory.hpp>
#include <service/IService.hpp>
#include <service/macros.hpp>

#include <unordered_set>

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION(sight::service::ut::ObjectServiceTest);

//------------------------------------------------------------------------------

namespace sight::service
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
    const std::string srvType("sight::service::ut::ISTest");
    const std::string srvImplementation1("sight::service::ut::STestNoData");
    const std::string srvImplementation2("sight::service::ut::STestNoData2");

    data::Integer::sptr obj1 = data::Integer::New();
    data::Integer::sptr obj2 = data::Integer::New();
    data::Integer::sptr obj3 = data::Integer::New();

    service::registry::ObjectService osr;
    auto service1 = service::extension::Factory::getDefault()->create(srvImplementation1);
    auto service2 = service::extension::Factory::getDefault()->create(srvImplementation2);
    auto service3 = service::extension::Factory::getDefault()->create(srvImplementation1);

    osr.registerService(service1);
    osr.registerService(service2);
    osr.registerService(service3);

    CPPUNIT_ASSERT_EQUAL(false, service1->hasObjectId("key1"));
    service1->registerObject("uid1", "key1", data::Access::inout);
    service1->registerObject("uid2", "key2", data::Access::inout);
    service1->registerObject("uid3", "key3", data::Access::inout);

    CPPUNIT_ASSERT_EQUAL(true, service1->hasObjectId("key1"));
    CPPUNIT_ASSERT_EQUAL(true, service1->hasObjectId("key2"));
    CPPUNIT_ASSERT_EQUAL(true, service1->hasObjectId("key3"));
    CPPUNIT_ASSERT_EQUAL(false, service1->hasObjectId("another_key"));

    CPPUNIT_ASSERT_EQUAL(std::string("uid1"), service1->getObjectId("key1"));
    CPPUNIT_ASSERT_EQUAL(std::string("uid2"), service1->getObjectId("key2"));
    CPPUNIT_ASSERT_EQUAL(std::string("uid3"), service1->getObjectId("key3"));
    CPPUNIT_ASSERT_THROW(service1->getObjectId("another_key"), core::Exception);

    service1->setObject(obj1, "key1", 0, data::Access::inout, false, false);
    service1->setObject(obj2, "key2", 0, data::Access::inout, false, false);
    service1->setObject(obj3, "key3", 0, data::Access::inout, false, false);

    CPPUNIT_ASSERT(obj1 == service1->getObject("key1", data::Access::inout));
    CPPUNIT_ASSERT(obj2 == service1->getObject("key2", data::Access::inout));
    CPPUNIT_ASSERT(obj3 == service1->getObject("key3", data::Access::inout));

    service2->setObject(obj1, "key1", 0, data::Access::inout, false, false);
    service2->setObject(obj2, "key2", 0, data::Access::inout, false, false);

    CPPUNIT_ASSERT(obj1 == service2->getObject("key1", data::Access::inout));
    CPPUNIT_ASSERT(obj2 == service2->getObject("key2", data::Access::inout));

    service3->setObject(obj3, "key3", 0, data::Access::inout, false, false);
    CPPUNIT_ASSERT(obj3 == service3->getObject("key3", data::Access::inout));

    // 3 services in total
    {
        auto servicesByType         = osr.getServices(srvType);
        auto servicesByTemplateType = osr.getServices<service::ut::ISTest>();

        CPPUNIT_ASSERT_EQUAL(size_t(3), servicesByType.size());
        CPPUNIT_ASSERT(std::equal(servicesByType.begin(), servicesByType.end(), servicesByTemplateType.begin()));
    }

    // 2 services of type "sight::service::ut::STestNoData"
    {
        auto servicesByType         = osr.getServices(srvImplementation1);
        auto servicesByTemplateType = osr.getServices<service::ut::STestNoData>();

        CPPUNIT_ASSERT_EQUAL(size_t(2), servicesByType.size());
        CPPUNIT_ASSERT(std::equal(servicesByType.begin(), servicesByType.end(), servicesByTemplateType.begin()));
    }

    // 1 service of type "sight::service::ut::STestNoData2"
    {
        auto servicesByType         = osr.getServices(srvImplementation2);
        auto servicesByTemplateType = osr.getServices<service::ut::STestNoData2>();

        CPPUNIT_ASSERT_EQUAL(size_t(1), servicesByType.size());
        CPPUNIT_ASSERT(std::equal(servicesByType.begin(), servicesByType.end(), servicesByTemplateType.begin()));
    }

    auto servicesByType = osr.getServices(srvType);
    CPPUNIT_ASSERT_EQUAL(size_t(3), servicesByType.size());

    CPPUNIT_ASSERT(servicesByType.find(service1) != servicesByType.end());
    CPPUNIT_ASSERT(servicesByType.find(service2) != servicesByType.end());
    CPPUNIT_ASSERT(servicesByType.find(service3) != servicesByType.end());

    // Remove key 1 from service 1 and check consistency
    service1->resetObject("key1", 0, data::Access::inout);
    CPPUNIT_ASSERT(nullptr == service1->getObject("key1", data::Access::inout));
    servicesByType = osr.getServices(srvType);
    CPPUNIT_ASSERT_EQUAL(size_t(3), servicesByType.size());

    osr.unregisterService(service1);
    servicesByType = osr.getServices(srvType);
    CPPUNIT_ASSERT_EQUAL(size_t(2), servicesByType.size());

    // Remove key 2 from service 2 and check consistency
    service2->resetObject("key2", 0, data::Access::inout);
    CPPUNIT_ASSERT(nullptr == service2->getObject("key2", data::Access::inout));

    servicesByType = osr.getServices(srvType);
    CPPUNIT_ASSERT_EQUAL(size_t(2), servicesByType.size());

    // Register key 2 to service 1 just for fun
    service1->setObject(obj2, "key2", 0, data::Access::inout, false, false);
    CPPUNIT_ASSERT(obj2 == service1->getObject("key2", data::Access::inout));

    osr.registerService(service1);
    servicesByType = osr.getServices(srvType);
    CPPUNIT_ASSERT_EQUAL(size_t(3), servicesByType.size());

    // Remove service 3 and check consistency
    osr.unregisterService(service3);
    servicesByType = osr.getServices(srvType);
    CPPUNIT_ASSERT_EQUAL(size_t(2), servicesByType.size());

    osr.unregisterService(service2);

    servicesByType = osr.getServices(srvType);
    CPPUNIT_ASSERT_EQUAL(size_t(1), servicesByType.size());

    osr.unregisterService(service1);
    servicesByType = osr.getServices(srvType);
    CPPUNIT_ASSERT(servicesByType.empty());
}

//------------------------------------------------------------------------------

void ObjectServiceTest::registerConnectionTest()
{
    service::registry::ObjectService osr;

    const std::string srvImplementation1("sight::service::ut::STest2Inputs");

    data::Integer::sptr obj1 = data::Integer::New();
    data::Integer::sptr obj2 = data::Integer::New();

    auto service1 = service::extension::Factory::getDefault()->create(srvImplementation1);

    auto worker         = core::thread::Worker::New();
    auto slotRegister   = core::com::newSlot(&ObjectServiceTest::registerService, this);
    auto slotUnregister = core::com::newSlot(&ObjectServiceTest::unregisterService, this);
    slotRegister->setWorker(worker);
    slotUnregister->setWorker(worker);

    osr.signal(service::registry::ObjectService::s_REGISTERED_SIG)->connect(slotRegister);
    osr.signal(service::registry::ObjectService::s_UNREGISTERED_SIG)->connect(slotUnregister);

    // Register callback test
    // Each time we wait the slot with a timeout to avoid blocking the test in case of failure
    service1->registerObject("data1", "key1", data::Access::out);
    osr.registerServiceOutput(obj1, "key1", service1);
    {
        std::unique_lock<std::mutex> lock(m_mutex);
        m_condition.wait_for(lock, std::chrono::milliseconds(1000), [this]{return m_ret == "uid1";});

        CPPUNIT_ASSERT_EQUAL(std::string("uid1"), m_ret);
        CPPUNIT_ASSERT(obj1 == m_obj);
    }

    service1->registerObject("data2", "key2", data::Access::out);
    osr.registerServiceOutput(obj2, "key2", service1);
    {
        std::unique_lock<std::mutex> lock(m_mutex);
        m_condition.wait_for(lock, std::chrono::milliseconds(1000), [this]{return m_ret == "uid2";});

        CPPUNIT_ASSERT_EQUAL(std::string("uid2"), m_ret);
        CPPUNIT_ASSERT(obj2 == m_obj);
    }

    // Unregister callback test
    osr.unregisterServiceOutput("data1", service1);
    {
        std::unique_lock<std::mutex> lock(m_mutex);
        m_condition.wait_for(lock, std::chrono::milliseconds(1000), [this]{return m_ret == "uid1";});

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

} //namespace sight::service
