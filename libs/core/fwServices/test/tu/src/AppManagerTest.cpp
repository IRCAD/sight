/************************************************************************
 *
 * Copyright (C) 2018-2019 IRCAD France
 * Copyright (C) 2018-2019 IHU Strasbourg
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

#include "AppManagerTest.hpp"

#include "fwServices/helper/ProxyConnections.hpp"
#include "fwServices/op/Add.hpp"
#include "fwServices/registry/ActiveWorkers.hpp"
#include "fwServices/registry/AppConfig.hpp"
#include "fwServices/registry/ObjectService.hpp"
#include "fwServices/registry/ServiceFactory.hpp"

#include <fwCom/Signal.hxx>

#include <fwData/Boolean.hpp>
#include <fwData/Image.hpp>
#include <fwData/Integer.hpp>

#include <fwTest/helper/wait.hpp>

#include <TestService.hpp>

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION( ::fwServices::ut::AppManagerTest );

namespace fwServices
{
namespace ut
{

//------------------------------------------------------------------------------

void AppManagerTest::setUp()
{
    // Set up context before running a test.

    ::fwServices::registry::ActiveWorkers::sptr activeWorkers = ::fwServices::registry::ActiveWorkers::getDefault();
    activeWorkers->initRegistry();
}

//------------------------------------------------------------------------------

void AppManagerTest::tearDown()
{
    // Clean up after the test run.
    if(m_appMgr)
    {
        // If everything went well, the manager should have been destroyed
        // This means a test failed, thus we need to clean everything properly, otherwise
        // We will get an assert from the destructor and we will not get the cppunit report in the console
        m_appMgr->destroy();
        m_appMgr = nullptr;
    }

    ::fwServices::registry::ActiveWorkers::sptr activeWorkers = ::fwServices::registry::ActiveWorkers::getDefault();
    activeWorkers->clearRegistry();
}

//------------------------------------------------------------------------------

void AppManagerTest::managerCreationTest()
{
    m_appMgr = std::unique_ptr< ::fwServices::AppManager >(new ::fwServices::AppManager);
    CPPUNIT_ASSERT(m_appMgr);

    m_appMgr->create();

    auto service  = m_appMgr->addService("::fwServices::ut::TestServiceImplementation", "srv1Uid", false, false);
    auto service2 = m_appMgr->addService< ::fwServices::ut::TestService >(
        "::fwServices::ut::TestServiceImplementation", "srv2Uid", true, false);
    auto service3 = m_appMgr->addService< ::fwServices::ut::TestService >(
        "::fwServices::ut::TestServiceImplementation", true, true);
    auto service4 = m_appMgr->addService("::fwServices::ut::TestServiceImplementation");

    auto service5 =
        ::fwServices::registry::ServiceFactory::getDefault()->create("::fwServices::ut::TestServiceImplementation");
    m_appMgr->addService(service5, true);

    CPPUNIT_ASSERT(service);
    CPPUNIT_ASSERT(service2);
    CPPUNIT_ASSERT(service3);
    CPPUNIT_ASSERT(service4);
    CPPUNIT_ASSERT(service5);

    CPPUNIT_ASSERT_EQUAL(false, service->isStarted());
    CPPUNIT_ASSERT_EQUAL(false, service2->isStarted());
    CPPUNIT_ASSERT_EQUAL(false, service3->isStarted());
    CPPUNIT_ASSERT_EQUAL(false, service4->isStarted());
    CPPUNIT_ASSERT_EQUAL(false, service5->isStarted());

    m_appMgr->startServices();
    CPPUNIT_ASSERT_EQUAL(false, service->isStarted());
    CPPUNIT_ASSERT_EQUAL(true, service2->isStarted());
    CPPUNIT_ASSERT_EQUAL(true, service3->isStarted());
    CPPUNIT_ASSERT_EQUAL(false, service4->isStarted());
    CPPUNIT_ASSERT_EQUAL(true, service5->isStarted());

    CPPUNIT_ASSERT_EQUAL(false, service2->getIsUpdated());
    CPPUNIT_ASSERT_EQUAL(true, service3->getIsUpdated());

    m_appMgr->destroy();

    CPPUNIT_ASSERT_EQUAL(false, service->isStarted());
    CPPUNIT_ASSERT_EQUAL(false, service2->isStarted());
    CPPUNIT_ASSERT_EQUAL(false, service3->isStarted());
    CPPUNIT_ASSERT_EQUAL(false, service4->isStarted());
    CPPUNIT_ASSERT_EQUAL(false, service5->isStarted());
}

//------------------------------------------------------------------------------

void AppManagerTest::managerWithObjectTest()
{
    m_appMgr = std::unique_ptr< ::fwServices::AppManager >(new ::fwServices::AppManager);
    CPPUNIT_ASSERT(m_appMgr);

    m_appMgr->create();

    const std::string imageId   = "imageTest";
    const std::string booleanId = "booleanTest";

    auto service  = m_appMgr->addService("::fwServices::ut::TestServiceImplementation", "srv1Uid", true, false);
    auto service2 = m_appMgr->addService< ::fwServices::ut::TestService >(
        "::fwServices::ut::TestServiceImplementation", "srv2Uid", true, false);
    auto service3 = m_appMgr->addService< ::fwServices::ut::TestService >(
        "::fwServices::ut::TestServiceImplementation", "srv3Uid", true, true);
    auto service4 = m_appMgr->addService< ::fwServices::ut::TestService >(
        "::fwServices::ut::TestServiceImplementation", true, true);

    CPPUNIT_ASSERT(service);
    CPPUNIT_ASSERT(service2);
    CPPUNIT_ASSERT(service3);
    CPPUNIT_ASSERT(service4);

    ::fwData::Image::sptr image     = ::fwData::Image::New();
    ::fwData::Boolean::sptr boolean = ::fwData::Boolean::New();

    service->registerObject(imageId, "data1", ::fwServices::IService::AccessType::INPUT);
    service2->registerObject(imageId, "data1", ::fwServices::IService::AccessType::INPUT, true);
    service3->registerObject(imageId, "data1", ::fwServices::IService::AccessType::INPUT, false, true);
    service3->registerObject(booleanId, "data2", ::fwServices::IService::AccessType::INPUT, true, true);
    service4->registerObject(booleanId, "data1", ::fwServices::IService::AccessType::INPUT, true);
    service4->registerObject(imageId, "data2", ::fwServices::IService::AccessType::INPUT, true);

    CPPUNIT_ASSERT_THROW(m_appMgr->startService(service), ::fwCore::Exception);

    m_appMgr->startServices();

    CPPUNIT_ASSERT_EQUAL(false, service->isStarted());
    CPPUNIT_ASSERT_EQUAL(false, service2->isStarted());
    CPPUNIT_ASSERT_EQUAL(true, service3->isStarted());
    CPPUNIT_ASSERT_EQUAL(false, service4->isStarted());

    CPPUNIT_ASSERT(nullptr == m_appMgr->getObject(imageId));
    CPPUNIT_ASSERT(nullptr == m_appMgr->getObject(booleanId));

    m_appMgr->addObject(image, imageId);

    fwTestWaitMacro(service->isStarted());
    CPPUNIT_ASSERT_EQUAL(true, service->isStarted());
    CPPUNIT_ASSERT_EQUAL(true, service2->isStarted());
    CPPUNIT_ASSERT_EQUAL(true, service3->isStarted());
    CPPUNIT_ASSERT_EQUAL(false, service4->isStarted());

    CPPUNIT_ASSERT(image == m_appMgr->getObject(imageId));

    m_appMgr->addObject(boolean, booleanId);

    CPPUNIT_ASSERT_EQUAL(true, service->isStarted());
    CPPUNIT_ASSERT_EQUAL(true, service2->isStarted());
    CPPUNIT_ASSERT_EQUAL(true, service3->isStarted());
    CPPUNIT_ASSERT_EQUAL(true, service4->isStarted());

    CPPUNIT_ASSERT(boolean == m_appMgr->getObject(booleanId));

    // add a service when object are already added
    auto service5 = ::fwServices::add("::fwServices::ut::TestServiceImplementation");
    service5->registerObject(imageId, "data1", ::fwServices::IService::AccessType::INPUT, false, false);
    service5->registerObject(booleanId, "data2", ::fwServices::IService::AccessType::INPUT, true, false);
    m_appMgr->addService(service5, true, true);

    fwTestWaitMacro(service5->isStarted());
    CPPUNIT_ASSERT_EQUAL(true, service5->isStarted());

    m_appMgr->removeObject(image, imageId);
    CPPUNIT_ASSERT(nullptr == m_appMgr->getObject(imageId));

    CPPUNIT_ASSERT_EQUAL(false, service->isStarted());
    CPPUNIT_ASSERT_EQUAL(false, service2->isStarted());
    CPPUNIT_ASSERT_EQUAL(true, service3->isStarted());
    CPPUNIT_ASSERT_EQUAL(false, service4->isStarted());
    CPPUNIT_ASSERT_EQUAL(false, service5->isStarted());

    m_appMgr->removeObject(boolean, booleanId);
    CPPUNIT_ASSERT(nullptr == m_appMgr->getObject(booleanId));

    CPPUNIT_ASSERT_EQUAL(false, service->isStarted());
    CPPUNIT_ASSERT_EQUAL(false, service2->isStarted());
    CPPUNIT_ASSERT_EQUAL(true, service3->isStarted());
    CPPUNIT_ASSERT_EQUAL(false, service4->isStarted());
    CPPUNIT_ASSERT_EQUAL(false, service5->isStarted());

    m_appMgr->destroy();
}
//------------------------------------------------------------------------------

void AppManagerTest::managerWithObjectConnectionTest()
{
    m_appMgr = std::unique_ptr< ::fwServices::AppManager >(new ::fwServices::AppManager);
    CPPUNIT_ASSERT(m_appMgr);

    m_appMgr->create();

    const std::string imageId      = "imageTest";
    const std::string booleanId    = "booleanTest";
    const std::string imageChannel = "imageChannel";

    auto service1 = m_appMgr->addService< ::fwServices::ut::TestSrvAutoconnect >(
        "::fwServices::ut::TestSrvAutoconnect", "myService1", true, false);
    auto service2 = m_appMgr->addService< ::fwServices::ut::TestSrvAutoconnect >(
        "::fwServices::ut::TestSrvAutoconnect", "myService2", true, false);
    auto service3 = m_appMgr->addService< ::fwServices::ut::TestSrvAutoconnect >(
        "::fwServices::ut::TestSrvAutoconnect", "myService3",  true, false);
    auto service4 = m_appMgr->addService< ::fwServices::ut::TestSrvAutoconnect >(
        "::fwServices::ut::TestSrvAutoconnect", "myService4", true, false);

    CPPUNIT_ASSERT(service1);
    CPPUNIT_ASSERT(service2);
    CPPUNIT_ASSERT(service3);
    CPPUNIT_ASSERT(service4);

    ::fwData::Image::sptr image     = ::fwData::Image::New();
    ::fwData::Boolean::sptr boolean = ::fwData::Boolean::New();

    service1->registerObject(imageId, "data1", ::fwServices::IService::AccessType::INPUT);
    service2->registerObject(imageId, "data1", ::fwServices::IService::AccessType::INPUT, true);
    service3->registerObject(imageId, "data1", ::fwServices::IService::AccessType::INPUT, false, true);
    service3->registerObject(booleanId, "data2", ::fwServices::IService::AccessType::INPUT, true, true);
    service4->registerObject(imageId, "data1", ::fwServices::IService::AccessType::INPUT, true);
    service4->registerObject(booleanId, "data2", ::fwServices::IService::AccessType::INPUT, true);

    m_appMgr->startServices();

    CPPUNIT_ASSERT_THROW(m_appMgr->startService(service1), ::fwCore::Exception);

    CPPUNIT_ASSERT_EQUAL(false, service1->isStarted());
    CPPUNIT_ASSERT_EQUAL(false, service2->isStarted());
    CPPUNIT_ASSERT_EQUAL(true, service3->isStarted());
    CPPUNIT_ASSERT_EQUAL(false, service4->isStarted());

    m_appMgr->addObject(image, imageId);

    CPPUNIT_ASSERT_EQUAL(true, service1->isStarted());
    CPPUNIT_ASSERT_EQUAL(true, service2->isStarted());
    CPPUNIT_ASSERT_EQUAL(true, service3->isStarted());
    CPPUNIT_ASSERT_EQUAL(false, service4->isStarted());

    auto sig = image->signal< ::fwData::Image::ModifiedSignalType >(::fwData::Image::s_MODIFIED_SIG);
    sig->emit();

    CPPUNIT_ASSERT_EQUAL(false, service1->getIsUpdated());
    CPPUNIT_ASSERT_EQUAL(true, service2->getIsUpdated());
    CPPUNIT_ASSERT_EQUAL(false, service3->getIsUpdated());
    CPPUNIT_ASSERT_EQUAL(false, service4->getIsUpdated());

    service2->resetIsUpdated();

    m_appMgr->removeObject(image, imageId);
    ::fwServices::helper::ProxyConnections connection(imageChannel);
    connection.addSignalConnection(imageId,  ::fwData::Image::s_MODIFIED_SIG);
    connection.addSlotConnection(service1->getID(), ::fwServices::ut::TestSrvAutoconnect::s_SLOT_1);
    m_appMgr->addProxyConnection(connection);
    m_appMgr->addObject(image, imageId);

    sig->emit();

    CPPUNIT_ASSERT_EQUAL(true, service1->getReceived());

    service1->resetReceive();

    m_appMgr->addObject(boolean, booleanId);
    auto sig2 = boolean->signal< ::fwData::Boolean::ModifiedSignalType >(::fwData::Boolean::s_MODIFIED_SIG);
    sig2->emit();

    CPPUNIT_ASSERT_EQUAL(false, service1->getReceived());
    CPPUNIT_ASSERT_EQUAL(false, service2->getReceived());
    CPPUNIT_ASSERT_EQUAL(true, service3->getReceived());
    CPPUNIT_ASSERT_EQUAL(true, service4->getReceived());

    service3->resetReceive();
    service4->resetReceive();

    m_appMgr->removeObject(boolean, booleanId);

    sig2->emit();

    CPPUNIT_ASSERT_EQUAL(false, service1->getReceived());
    CPPUNIT_ASSERT_EQUAL(false, service2->getReceived());
    CPPUNIT_ASSERT_EQUAL(false, service3->getReceived());
    CPPUNIT_ASSERT_EQUAL(false, service4->getReceived());

    m_appMgr->destroy();
}

//------------------------------------------------------------------------------

void AppManagerTest::managerWithServiceConnectionTest()
{
    m_appMgr = std::unique_ptr< ::fwServices::AppManager >(new ::fwServices::AppManager);
    CPPUNIT_ASSERT(m_appMgr);

    m_appMgr->create();

    const std::string channel = "channel";

    auto service1 = m_appMgr->addService< ::fwServices::ut::TestSrvAutoconnect >(
        "::fwServices::ut::TestSrvAutoconnect", true);
    auto service2 = m_appMgr->addService< ::fwServices::ut::TestSrvAutoconnect >(
        "::fwServices::ut::TestSrvAutoconnect", true);
    auto service3 = m_appMgr->addService< ::fwServices::ut::TestSrvAutoconnect >(
        "::fwServices::ut::TestSrvAutoconnect", true);
    auto service4 = m_appMgr->addService< ::fwServices::ut::TestSrvAutoconnect >(
        "::fwServices::ut::TestSrvAutoconnect", true);

    ::fwServices::helper::ProxyConnections connection1;
    connection1.addSignalConnection(service1->getID(), ::fwServices::ut::TestSrvAutoconnect::s_SIG_1);
    connection1.addSlotConnection(service2->getID(), ::fwServices::ut::TestSrvAutoconnect::s_SLOT_1);
    m_appMgr->addProxyConnection(connection1);

    ::fwServices::helper::ProxyConnections connection2(channel);
    connection2.addSignalConnection(service1->getID(), ::fwServices::ut::TestSrvAutoconnect::s_UPDATED_SIG);
    connection2.addSlotConnection(service3->getID(), ::fwServices::ut::TestSrvAutoconnect::s_SLOT_1);
    m_appMgr->addProxyConnection(connection2);

    CPPUNIT_ASSERT_NO_THROW(m_appMgr->startService(service1));
    CPPUNIT_ASSERT_NO_THROW(m_appMgr->startService(service2));
    CPPUNIT_ASSERT_NO_THROW(m_appMgr->startService(service3));
    CPPUNIT_ASSERT_NO_THROW(m_appMgr->startService(service4));

    CPPUNIT_ASSERT_EQUAL(true, service1->isStarted());
    CPPUNIT_ASSERT_EQUAL(true, service2->isStarted());
    CPPUNIT_ASSERT_EQUAL(true, service3->isStarted());
    CPPUNIT_ASSERT_EQUAL(true, service4->isStarted());

    auto sig = service1->signal< ::fwServices::ut::TestSrvAutoconnect::MsgSentSignalType >(
        ::fwServices::ut::TestSrvAutoconnect::s_SIG_1);
    sig->emit(3);

    CPPUNIT_ASSERT_EQUAL(false, service1->getReceived());
    CPPUNIT_ASSERT_EQUAL(true, service2->getReceived());
    CPPUNIT_ASSERT_EQUAL(false, service3->getReceived());
    CPPUNIT_ASSERT_EQUAL(false, service4->getReceived());

    service2->resetReceive();

    service1->update(); // emit signal 'updated'

    fwTestWaitMacro(service3->getReceived());
    CPPUNIT_ASSERT_EQUAL(false, service1->getReceived());
    CPPUNIT_ASSERT_EQUAL(false, service2->getReceived());
    CPPUNIT_ASSERT_EQUAL(true, service3->getReceived());
    CPPUNIT_ASSERT_EQUAL(false, service4->getReceived());

    m_appMgr->destroy();
}

//------------------------------------------------------------------------------

void AppManagerTest::managerWithOutputCreationTest()
{
    m_appMgr = std::unique_ptr< ::fwServices::AppManager >(new ::fwServices::AppManager);
    CPPUNIT_ASSERT(m_appMgr);

    m_appMgr->create();

    const std::string integerId           = "integerId";
    const std::string generatedIntegerId  = "generatedIntegerId";
    const std::string generatedInteger2Id = "generatedInteger2Id";

    ::fwData::Integer::sptr integer1 = ::fwData::Integer::New(15);

    auto service1 = m_appMgr->addService< ::fwServices::ut::TestServiceWithData >(
        "::fwServices::ut::TestServiceWithData", true, true);
    auto service2 = m_appMgr->addService< ::fwServices::ut::TestServiceWithData >(
        "::fwServices::ut::TestServiceWithData", true);
    auto service3 = m_appMgr->addService< ::fwServices::ut::TestServiceWithData >(
        "::fwServices::ut::TestServiceWithData", true);
    auto service4 = m_appMgr->addService< ::fwServices::ut::TestServiceWithData >(
        "::fwServices::ut::TestServiceWithData", true);

    CPPUNIT_ASSERT(service1);
    CPPUNIT_ASSERT(service2);
    CPPUNIT_ASSERT(service3);
    CPPUNIT_ASSERT(service4);

    m_appMgr->startServices();
    CPPUNIT_ASSERT_EQUAL(false, service1->isStarted());
    CPPUNIT_ASSERT_EQUAL(false, service3->isStarted());
    CPPUNIT_ASSERT_EQUAL(false, service4->isStarted());
    CPPUNIT_ASSERT_EQUAL(false, service2->isStarted());

    service1->setObjectId(::fwServices::ut::TestServiceWithData::s_INPUT, integerId);
    service1->setObjectId(::fwServices::ut::TestServiceWithData::s_OUTPUT, generatedIntegerId);
    service2->setObjectId(::fwServices::ut::TestServiceWithData::s_INPUT, generatedIntegerId);
    service3->setObjectId(::fwServices::ut::TestServiceWithData::s_INPUT, integerId);
    service3->setObjectId(::fwServices::ut::TestServiceWithData::s_OUTPUT, generatedInteger2Id);
    service4->setObjectId(::fwServices::ut::TestServiceWithData::s_INPUT, generatedInteger2Id);

    m_appMgr->addObject(integer1, integerId);

    CPPUNIT_ASSERT_EQUAL(true, service1->isStarted());
    CPPUNIT_ASSERT_EQUAL(true, service3->isStarted());
    CPPUNIT_ASSERT_EQUAL(false, service4->isStarted());
    fwTestWaitMacro(service2->isStarted()); // wait until service1 generate its output to start service2
    CPPUNIT_ASSERT_EQUAL(true, service2->isStarted());

    auto integer2 = service1->getOutput< ::fwData::Integer >(::fwServices::ut::TestServiceWithData::s_OUTPUT);
    CPPUNIT_ASSERT(integer2);
    CPPUNIT_ASSERT_EQUAL(integer1->getValue(), integer2->value());
    CPPUNIT_ASSERT(nullptr ==
                   service3->getOutput< ::fwData::Integer >(::fwServices::ut::TestServiceWithData::s_OUTPUT));

    service3->update().wait();

    auto integer3 = service3->getOutput< ::fwData::Integer >(::fwServices::ut::TestServiceWithData::s_OUTPUT);

    CPPUNIT_ASSERT(integer3);
    CPPUNIT_ASSERT_EQUAL(integer1->getValue(), integer3->getValue());
    fwTestWaitMacro(service4->isStarted());
    CPPUNIT_ASSERT_EQUAL(true, service4->isStarted());

    m_appMgr->destroy();
}

//------------------------------------------------------------------------------

void AppManagerTest::managerWithGroupTest()
{
    m_appMgr = std::unique_ptr< ::fwServices::AppManager >(new ::fwServices::AppManager);
    CPPUNIT_ASSERT(m_appMgr);

    m_appMgr->create();

    const std::string integerId0          = "integerId0";
    const std::string integerId1          = "integerId1";
    const std::string integerId2          = "integerId2";
    const std::string integerId3          = "integerId3";
    const std::string generatedIntegerId  = "generatedIntegerId";
    const std::string generatedInteger2Id = "generatedInteger2Id";

    ::fwData::Integer::sptr integer0 = ::fwData::Integer::New(1);
    ::fwData::Integer::sptr integer1 = ::fwData::Integer::New(15);
    ::fwData::Integer::sptr integer2 = ::fwData::Integer::New(25);
    ::fwData::Integer::sptr integer3 = ::fwData::Integer::New(28);

    auto service1 = m_appMgr->addService< ::fwServices::ut::TestServiceWithData >(
        "::fwServices::ut::TestServiceWithData", true);
    service1->registerGroup();

    CPPUNIT_ASSERT_EQUAL(false, service1->hasAllRequiredObjects());
    CPPUNIT_ASSERT_EQUAL(size_t(2), service1->getKeyGroupSize(::fwServices::ut::TestServiceWithData::s_INOUT_GROUP));
    service1->setObjectId(::fwServices::ut::TestServiceWithData::s_INPUT, integerId0);
    service1->setObjectId(::fwServices::ut::TestServiceWithData::s_INOUT_GROUP, 0, integerId1);
    service1->setObjectId(::fwServices::ut::TestServiceWithData::s_INOUT_GROUP, 1, integerId2);
    service1->setObjectId(::fwServices::ut::TestServiceWithData::s_INOUT_GROUP, 2, integerId3);
    CPPUNIT_ASSERT_EQUAL(size_t(3), service1->getKeyGroupSize(::fwServices::ut::TestServiceWithData::s_INOUT_GROUP));
    m_appMgr->addObject(integer0, integerId0);

    m_appMgr->startServices();

    CPPUNIT_ASSERT_EQUAL(true, service1->hasObjectId(
                             KEY_GROUP_NAME(::fwServices::ut::TestServiceWithData::s_INOUT_GROUP, 0)));
    CPPUNIT_ASSERT_EQUAL(integerId1, service1->getObjectId(
                             KEY_GROUP_NAME(::fwServices::ut::TestServiceWithData::s_INOUT_GROUP, 0)));
    CPPUNIT_ASSERT_EQUAL(true, service1->hasObjectId(
                             KEY_GROUP_NAME(::fwServices::ut::TestServiceWithData::s_INOUT_GROUP, 1)));
    CPPUNIT_ASSERT_EQUAL(integerId2, service1->getObjectId(
                             KEY_GROUP_NAME(::fwServices::ut::TestServiceWithData::s_INOUT_GROUP, 1)));
    CPPUNIT_ASSERT_EQUAL(true, service1->hasObjectId(
                             KEY_GROUP_NAME(::fwServices::ut::TestServiceWithData::s_INOUT_GROUP, 2)));
    CPPUNIT_ASSERT_EQUAL(integerId3, service1->getObjectId(
                             KEY_GROUP_NAME(::fwServices::ut::TestServiceWithData::s_INOUT_GROUP, 2)));

    m_appMgr->addObject(integer1, integerId1);
    CPPUNIT_ASSERT_EQUAL(false, service1->hasAllRequiredObjects());
    m_appMgr->addObject(integer2, integerId2);
    CPPUNIT_ASSERT_EQUAL(true, service1->hasAllRequiredObjects());
    m_appMgr->addObject(integer3, integerId3);
    CPPUNIT_ASSERT_EQUAL(true, service1->hasAllRequiredObjects());

    m_appMgr->removeObject(integer3, integerId3);
    CPPUNIT_ASSERT_EQUAL(true, service1->hasAllRequiredObjects());
    m_appMgr->removeObject(integer2, integerId2);
    CPPUNIT_ASSERT_EQUAL(false, service1->hasAllRequiredObjects());

    service1->unregisterObject(integerId3);
    CPPUNIT_ASSERT_EQUAL(false, service1->hasObjectId(
                             KEY_GROUP_NAME(::fwServices::ut::TestServiceWithData::s_INOUT_GROUP, 2)));

    service1->unregisterObject(integerId2);
    CPPUNIT_ASSERT_EQUAL(false, service1->hasObjectId(
                             KEY_GROUP_NAME(::fwServices::ut::TestServiceWithData::s_INOUT_GROUP, 1)));

    service1->unregisterObject(integerId1);
    CPPUNIT_ASSERT_EQUAL(false, service1->hasObjectId(
                             KEY_GROUP_NAME(::fwServices::ut::TestServiceWithData::s_INOUT_GROUP, 0)));

    m_appMgr->destroy();
}

//------------------------------------------------------------------------------

void AppManagerTest::managerWithInputsTest()
{
    const std::string INPUT_OBJ1         = "object1";
    const std::string INPUT_OBJ2         = "object2";
    const std::string OPTIONAL_INPUT_OBJ = "optionalObject";
    const std::string INPUT_CHANNEL1     = "channel1";
    const std::string INPUT_CHANNEL2     = "channel2";
    const std::string INPUT_STR          = "inputStr";

    auto appMgr = std::unique_ptr< AppManagerForTest >(new AppManagerForTest);
    CPPUNIT_ASSERT(appMgr);

    appMgr->create();

    CPPUNIT_ASSERT(appMgr->checkInputs());

    appMgr->requireInput(INPUT_STR, AppManagerForTest::InputType::OTHER);

    CPPUNIT_ASSERT(!appMgr->checkInputs());

    const std::string STR = "MyString";
    appMgr->replaceInput(INPUT_STR, STR);

    CPPUNIT_ASSERT(appMgr->checkInputs());
    CPPUNIT_ASSERT_EQUAL(STR, appMgr->getInputID(INPUT_STR));

    appMgr->requireInput(INPUT_OBJ1, AppManagerForTest::InputType::OBJECT);
    appMgr->requireInput(INPUT_OBJ2, AppManagerForTest::InputType::OBJECT);
    appMgr->requireInput(OPTIONAL_INPUT_OBJ, AppManagerForTest::InputType::OBJECT, "::fwData::Image");

    CPPUNIT_ASSERT(!appMgr->checkInputs());

    ::fwData::Boolean::sptr boolean1 = ::fwData::Boolean::New();
    ::fwData::Boolean::sptr boolean2 = ::fwData::Boolean::New();

    appMgr->replaceInput(INPUT_OBJ1, boolean1->getID());
    appMgr->replaceInput(INPUT_OBJ2, boolean2->getID());

    CPPUNIT_ASSERT(appMgr->checkInputs());
    CPPUNIT_ASSERT_EQUAL(boolean1->getID(), appMgr->getInputID(INPUT_OBJ1));
    CPPUNIT_ASSERT_EQUAL(boolean2->getID(), appMgr->getInputID(INPUT_OBJ2));
    CPPUNIT_ASSERT_EQUAL(appMgr->getInputID(INPUT_OBJ2), appMgr->getInputID(INPUT_OBJ2));

    const auto image = appMgr->getObject(appMgr->getInputID(INPUT_OBJ2));
    CPPUNIT_ASSERT(image);

    appMgr->requireInput(INPUT_CHANNEL2, AppManagerForTest::InputType::CHANNEL, INPUT_CHANNEL2);
    CPPUNIT_ASSERT(appMgr->checkInputs());

    CPPUNIT_ASSERT_EQUAL(INPUT_CHANNEL2, INPUT_CHANNEL2);

    appMgr->replaceInput(INPUT_CHANNEL2, appMgr->getInputID(INPUT_CHANNEL2));
    CPPUNIT_ASSERT_EQUAL(INPUT_CHANNEL2, INPUT_CHANNEL2);
    CPPUNIT_ASSERT(appMgr->checkInputs());

    appMgr->requireInput(INPUT_CHANNEL1, AppManagerForTest::InputType::CHANNEL);
    CPPUNIT_ASSERT(!appMgr->checkInputs());

    appMgr->replaceInput(INPUT_CHANNEL1, appMgr->getInputID("INPUT_CHANNEL1"));
    CPPUNIT_ASSERT(appMgr->checkInputs());

    // Check that getInputID return the same string on each iteration
    const std::string testSTR = appMgr->getInputID("myString");
    for (size_t i = 0; i < 10; ++i)
    {
        CPPUNIT_ASSERT_EQUAL(testSTR, appMgr->getInputID("myString"));
    }

    // Check that getInputID does not return the same string on another instance of the manager
    auto appMgr2 = std::unique_ptr< AppManagerForTest >(new AppManagerForTest);
    CPPUNIT_ASSERT(appMgr2);
    appMgr2->create();

    CPPUNIT_ASSERT(appMgr->getInputID("myString") != appMgr2->getInputID("myString"));

    appMgr->destroy();
    appMgr2->destroy();
}

//------------------------------------------------------------------------------

} //namespace ut

} //namespace fwServices
