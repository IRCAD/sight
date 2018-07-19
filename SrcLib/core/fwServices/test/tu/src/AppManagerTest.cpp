/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2018.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "AppManagerTest.hpp"

#include "fwServices/registry/ActiveWorkers.hpp"
#include "fwServices/registry/AppConfig.hpp"
#include "fwServices/registry/ObjectService.hpp"

#include <fwCom/Signal.hxx>

#include <fwData/Boolean.hpp>
#include <fwData/Image.hpp>

#include <fwTest/helper/wait.hpp>

#include <TestService.hpp>

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION( ::fwServices::ut::AppManagerTest );

namespace fwServices
{
namespace ut
{

#define WAIT_SERVICE_STARTED(srv)  \
    fwTestWaitMacro(::fwTools::fwID::getObject(srv) != nullptr && \
                    ::fwServices::IService::dynamicCast(::fwTools::fwID::getObject(srv))->getStatus() \
                    == ::fwServices::IService::STARTED)

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

    auto service  = m_appMgr->registerService("::fwServices::ut::TestServiceImplementation", "srv1Uid");
    auto service2 = m_appMgr->registerService< ::fwServices::ut::TestService >(
        "::fwServices::ut::TestServiceImplementation", "srv2Uid", true);
    auto service3 = m_appMgr->registerService< ::fwServices::ut::TestService >(
        "::fwServices::ut::TestServiceImplementation", "", true, true);

    CPPUNIT_ASSERT(service);
    CPPUNIT_ASSERT(service2);
    CPPUNIT_ASSERT(service3);

    CPPUNIT_ASSERT_EQUAL(false, service->isStarted());
    CPPUNIT_ASSERT_EQUAL(false, service2->isStarted());
    CPPUNIT_ASSERT_EQUAL(false, service3->isStarted());

    m_appMgr->startServices();
    CPPUNIT_ASSERT_EQUAL(false, service->isStarted());
    CPPUNIT_ASSERT_EQUAL(true, service2->isStarted());
    CPPUNIT_ASSERT_EQUAL(true, service3->isStarted());

    CPPUNIT_ASSERT_EQUAL(false, service2->getIsUpdated());
    CPPUNIT_ASSERT_EQUAL(true, service3->getIsUpdated());

    m_appMgr->destroy();

    CPPUNIT_ASSERT_EQUAL(false, service->isStarted());
    CPPUNIT_ASSERT_EQUAL(false, service2->isStarted());
    CPPUNIT_ASSERT_EQUAL(false, service3->isStarted());
}

//------------------------------------------------------------------------------

void AppManagerTest::managerWithObjectTest()
{
    m_appMgr = std::unique_ptr< ::fwServices::AppManager >(new ::fwServices::AppManager);
    CPPUNIT_ASSERT(m_appMgr);

    m_appMgr->create();

    const std::string imageId   = "imageTest";
    const std::string booleanId = "booleanTest";

    auto service  = m_appMgr->registerService("::fwServices::ut::TestServiceImplementation", "srv1Uid", true);
    auto service2 = m_appMgr->registerService< ::fwServices::ut::TestService >(
        "::fwServices::ut::TestServiceImplementation", "srv2Uid", true);
    auto service3 = m_appMgr->registerService< ::fwServices::ut::TestService >(
        "::fwServices::ut::TestServiceImplementation", "srv3Uid", true, true);
    auto service4 = m_appMgr->registerService< ::fwServices::ut::TestService >(
        "::fwServices::ut::TestServiceImplementation", "", true, true);

    CPPUNIT_ASSERT(service);
    CPPUNIT_ASSERT(service2);
    CPPUNIT_ASSERT(service3);
    CPPUNIT_ASSERT(service4);

    ::fwData::Image::sptr image     = ::fwData::Image::New();
    ::fwData::Boolean::sptr boolean = ::fwData::Boolean::New();

    m_appMgr->registerObject(service, imageId, "data1", ::fwServices::IService::AccessType::INPUT);
    m_appMgr->registerObject(service2, imageId, "data1", ::fwServices::IService::AccessType::INPUT, true);
    m_appMgr->registerObject(service3, imageId, "data1", ::fwServices::IService::AccessType::INPUT, false, true);
    m_appMgr->registerObject(service3, booleanId, "data2", ::fwServices::IService::AccessType::INPUT, true, true);
    m_appMgr->registerObject(service4, booleanId, "data1", ::fwServices::IService::AccessType::INPUT, true);
    m_appMgr->registerObject(service4, imageId, "data2", ::fwServices::IService::AccessType::INPUT, true);

    CPPUNIT_ASSERT_THROW(m_appMgr->startService(service), ::fwCore::Exception);

    m_appMgr->startServices();

    CPPUNIT_ASSERT_EQUAL(false, service->isStarted());
    CPPUNIT_ASSERT_EQUAL(false, service2->isStarted());
    CPPUNIT_ASSERT_EQUAL(true, service3->isStarted());
    CPPUNIT_ASSERT_EQUAL(false, service4->isStarted());

    m_appMgr->addObject(image, imageId);

    CPPUNIT_ASSERT_EQUAL(true, service->isStarted());
    CPPUNIT_ASSERT_EQUAL(true, service2->isStarted());
    CPPUNIT_ASSERT_EQUAL(true, service3->isStarted());
    CPPUNIT_ASSERT_EQUAL(false, service4->isStarted());

    m_appMgr->addObject(boolean, booleanId);

    CPPUNIT_ASSERT_EQUAL(true, service->isStarted());
    CPPUNIT_ASSERT_EQUAL(true, service2->isStarted());
    CPPUNIT_ASSERT_EQUAL(true, service3->isStarted());
    CPPUNIT_ASSERT_EQUAL(true, service4->isStarted());

    m_appMgr->removeObject(image, imageId);

    CPPUNIT_ASSERT_EQUAL(false, service->isStarted());
    CPPUNIT_ASSERT_EQUAL(false, service2->isStarted());
    CPPUNIT_ASSERT_EQUAL(true, service3->isStarted());
    CPPUNIT_ASSERT_EQUAL(false, service4->isStarted());

    m_appMgr->removeObject(boolean, booleanId);

    CPPUNIT_ASSERT_EQUAL(false, service->isStarted());
    CPPUNIT_ASSERT_EQUAL(false, service2->isStarted());
    CPPUNIT_ASSERT_EQUAL(true, service3->isStarted());
    CPPUNIT_ASSERT_EQUAL(false, service4->isStarted());

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

    auto service1 = m_appMgr->registerService< ::fwServices::ut::TestSrvAutoconnect >(
        "::fwServices::ut::TestSrvAutoconnect", "", true);
    auto service2 = m_appMgr->registerService< ::fwServices::ut::TestSrvAutoconnect >(
        "::fwServices::ut::TestSrvAutoconnect", "", true);
    auto service3 = m_appMgr->registerService< ::fwServices::ut::TestSrvAutoconnect >(
        "::fwServices::ut::TestSrvAutoconnect", "", true);
    auto service4 = m_appMgr->registerService< ::fwServices::ut::TestSrvAutoconnect >(
        "::fwServices::ut::TestSrvAutoconnect", "", true);

    CPPUNIT_ASSERT(service1);
    CPPUNIT_ASSERT(service2);
    CPPUNIT_ASSERT(service3);
    CPPUNIT_ASSERT(service4);

    ::fwData::Image::sptr image     = ::fwData::Image::New();
    ::fwData::Boolean::sptr boolean = ::fwData::Boolean::New();

    m_appMgr->registerObject(service1, imageId, "data1", ::fwServices::IService::AccessType::INPUT);
    m_appMgr->registerObject(service2, imageId, "data1", ::fwServices::IService::AccessType::INPUT, true);
    m_appMgr->registerObject(service3, imageId, "data1", ::fwServices::IService::AccessType::INPUT, false, true);
    m_appMgr->registerObject(service3, booleanId, "data2", ::fwServices::IService::AccessType::INPUT, true, true);
    m_appMgr->registerObject(service4, imageId, "data1", ::fwServices::IService::AccessType::INPUT, true);
    m_appMgr->registerObject(service4, booleanId, "data2", ::fwServices::IService::AccessType::INPUT, true);

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
    m_appMgr->connectObjectSignal(imageChannel, imageId, ::fwData::Image::s_MODIFIED_SIG);
    m_appMgr->connectSlot(imageChannel, service1, ::fwServices::ut::TestSrvAutoconnect::s_SLOT_1);
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

    const std::string channel1 = "channel1";
    const std::string channel2 = "channel2";

    auto service1 = m_appMgr->registerService< ::fwServices::ut::TestSrvAutoconnect >(
        "::fwServices::ut::TestSrvAutoconnect", "", true);
    auto service2 = m_appMgr->registerService< ::fwServices::ut::TestSrvAutoconnect >(
        "::fwServices::ut::TestSrvAutoconnect", "", true);
    auto service3 = m_appMgr->registerService< ::fwServices::ut::TestSrvAutoconnect >(
        "::fwServices::ut::TestSrvAutoconnect", "", true);
    auto service4 = m_appMgr->registerService< ::fwServices::ut::TestSrvAutoconnect >(
        "::fwServices::ut::TestSrvAutoconnect", "", true);

    m_appMgr->connectSignal(channel1, service1, ::fwServices::ut::TestSrvAutoconnect::s_SIG_1);
    m_appMgr->connectSignal(channel2, service1, ::fwServices::ut::TestSrvAutoconnect::s_UPDATED_SIG);
    m_appMgr->connectSlot(channel1, service2, ::fwServices::ut::TestSrvAutoconnect::s_SLOT_1);
    m_appMgr->connectSlot(channel2, service3, ::fwServices::ut::TestSrvAutoconnect::s_SLOT_1);

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
}

//------------------------------------------------------------------------------

} //namespace ut

} //namespace fwServices
