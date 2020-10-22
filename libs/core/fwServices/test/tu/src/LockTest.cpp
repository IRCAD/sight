/************************************************************************
 *
 * Copyright (C) 2009-2020 IRCAD France
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

#include "LockTest.hpp"

#include <fwServices/IService.hpp>
#include <fwServices/macros.hpp>
#include <fwServices/op/Add.hpp>
#include <fwServices/op/Get.hpp>

#include <fwData/Image.hpp>
#include <fwData/Integer.hpp>
#include <fwData/Mesh.hpp>
#include <fwData/mt/locked_ptr.hpp>
#include <fwData/mt/weak_ptr.hpp>

#include <fwTest/generator/Image.hpp>
#include <fwTest/generator/Mesh.hpp>

#include <fwThread/Worker.hpp>

#include <atomic>
#include <chrono>
#include <thread>

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION( ::fwServices::ut::LockTest );

//------------------------------------------------------------------------------

namespace fwServices
{
namespace ut
{

class LockedService : public ::fwServices::IService
{
public:

    static const ::fwServices::IService::KeyType s_INPUT;
    static const ::fwServices::IService::KeyType s_INOUT;
    static const ::fwServices::IService::KeyType s_OUTPUT;

    fwCoreServiceMacro(LockedService, ::fwServices::IService)

    LockedService() noexcept
    {
        this->registerObject(s_INPUT, AccessType::INPUT, false, false);
        this->registerObject(s_INOUT, AccessType::INOUT, false, false);
        this->registerObject(s_OUTPUT, AccessType::OUTPUT, false, true);
    }

    std::atomic_bool m_started {false};
    std::atomic_bool m_stopped {false};
    std::atomic_int64_t m_input {-1};

    //------------------------------------------------------------------------------

    virtual void starting() final
    {
        // Reading should not be blocked by other reader
        auto weakInput   = this->getWeakInput< ::fwData::Integer >(s_INPUT);
        auto sharedInput = weakInput.lock();

        // Simulate working....
        std::this_thread::sleep_for(std::chrono::seconds(2));

        m_input   = sharedInput->getValue();
        m_started = true;
    }

    //------------------------------------------------------------------------------

    virtual void stopping() final
    {
        // Reading should not be blocked by other reader
        auto weakOutput   = this->getWeakOutput< ::fwData::Integer >(s_OUTPUT);
        auto sharedOutput = weakOutput.lock();

        sharedOutput->setValue(-1);

        // Simulate working....
        std::this_thread::sleep_for(std::chrono::seconds(2));

        sharedOutput->setValue(1);
        m_stopped = true;
    }

    //------------------------------------------------------------------------------

    virtual void configuring() final
    {

    }

    //------------------------------------------------------------------------------

    virtual void reconfiguring() final
    {

    }

    //------------------------------------------------------------------------------

    virtual void updating() final
    {

    }
};

const ::fwServices::IService::KeyType LockedService::s_INPUT  = "input";
const ::fwServices::IService::KeyType LockedService::s_INOUT  = "inout";
const ::fwServices::IService::KeyType LockedService::s_OUTPUT = "output";

fwServicesRegisterMacro( ::fwServices::IService, ::fwServices::ut::LockedService)

//------------------------------------------------------------------------------

void LockTest::setUp()
{
    // Set up context before running a test.
}

//------------------------------------------------------------------------------

void LockTest::tearDown()
{
    // Clean up after the test run.
    // unregister the services that have not been unregistered because a test failed.

    const auto& services = ::fwServices::OSR::getServices< ::fwServices::IService >();
    for (auto service: services)
    {
        if (service->isStarted())
        {
            service->stop();
        }

        ::fwServices::OSR::unregisterService(service);
    }
}

//------------------------------------------------------------------------------

void LockTest::testScopedLock()
{
    // Add the service
    ::fwServices::IService::sptr lockedService = ::fwServices::add("::fwServices::ut::LockedService");
    CPPUNIT_ASSERT(lockedService);

    // Create the data
    ::fwData::Integer::csptr input = ::fwData::Integer::New(0);
    ::fwData::Integer::sptr inout  = ::fwData::Integer::New(0);
    ::fwData::Integer::sptr output = ::fwData::Integer::New(0);

    // Register the data
    lockedService->registerInput(input, ::fwServices::ut::LockedService::s_INPUT);
    lockedService->registerInOut(inout, ::fwServices::ut::LockedService::s_INOUT);
    lockedService->setOutput(::fwServices::ut::LockedService::s_OUTPUT, output);
    CPPUNIT_ASSERT_EQUAL(true, lockedService->hasAllRequiredObjects());

    // Test basic scoped lock
    ::fwData::mt::weak_ptr< const ::fwData::Integer > weakInput(input);
    {
        auto sharedInput = weakInput.lock();
        CPPUNIT_ASSERT_EQUAL(input, sharedInput.get_shared());
    }

    ::fwData::mt::weak_ptr< ::fwData::Integer > weakInOut(inout);
    {
        auto sharedInOut = weakInOut.lock();
        CPPUNIT_ASSERT_EQUAL(inout, sharedInOut.get_shared());
    }

    ::fwData::mt::weak_ptr< ::fwData::Integer > weakOutput(output);
    {
        auto sharedOutput = weakOutput.lock();
        CPPUNIT_ASSERT_EQUAL(output, sharedOutput.get_shared());
    }

    // Test basic scoped lock from service getters
    weakInput = lockedService->getWeakInput< ::fwData::Integer >(::fwServices::ut::LockedService::s_INPUT);
    {
        auto sharedInput = weakInput.lock();
        CPPUNIT_ASSERT_EQUAL(input, sharedInput.get_shared());
    }

    weakInOut = lockedService->getWeakInOut< ::fwData::Integer >(::fwServices::ut::LockedService::s_INOUT);
    {
        auto sharedInOut = weakInOut.lock();
        CPPUNIT_ASSERT_EQUAL(inout, sharedInOut.get_shared());
    }

    weakOutput = lockedService->getWeakOutput< ::fwData::Integer >(::fwServices::ut::LockedService::s_OUTPUT);
    {
        auto sharedOutput = weakOutput.lock();
        CPPUNIT_ASSERT_EQUAL(output, sharedOutput.get_shared());
    }

    // Test basic scoped lock from service direct locker
    {
        auto sharedInput = lockedService->getLockedInput< ::fwData::Integer >(::fwServices::ut::LockedService::s_INPUT);
        CPPUNIT_ASSERT_EQUAL(input, sharedInput.get_shared());
    }

    {
        auto sharedInOut = lockedService->getLockedInOut< ::fwData::Integer >(::fwServices::ut::LockedService::s_INOUT);
        CPPUNIT_ASSERT_EQUAL(inout, sharedInOut.get_shared());
    }

    {
        auto sharedOutput = lockedService->getLockedOutput< ::fwData::Integer >(
            ::fwServices::ut::LockedService::s_OUTPUT);
        CPPUNIT_ASSERT_EQUAL(output, sharedOutput.get_shared());
    }

    // cleanup
    ::fwServices::OSR::unregisterService(lockedService);
}

//------------------------------------------------------------------------------

void LockTest::testDumpLock()
{
    ::fwData::Image::sptr image = ::fwData::Image::New();

    ::fwTest::generator::Image::generateRandomImage(image, ::fwTools::Type::s_UINT8);

    // Add the service
    ::fwServices::IService::sptr lockedService = ::fwServices::add("::fwServices::ut::LockedService");
    CPPUNIT_ASSERT(lockedService);

    lockedService->registerInput(image, ::fwServices::ut::LockedService::s_INPUT);

    {
        auto sharedInput = lockedService->getLockedInput< ::fwData::Image >(::fwServices::ut::LockedService::s_INPUT);
        CPPUNIT_ASSERT(image == sharedInput.get_shared());
        // check if the image is properly locked for dump
        CPPUNIT_ASSERT_NO_THROW(image->getBuffer());
    }
    CPPUNIT_ASSERT_THROW(image->getBuffer(), ::fwData::Exception);

    ::fwData::Mesh::sptr mesh = ::fwData::Mesh::New();

    lockedService->registerInput(mesh, ::fwServices::ut::LockedService::s_INPUT);

    {
        auto sharedInput = lockedService->getLockedInput< ::fwData::Mesh >(::fwServices::ut::LockedService::s_INPUT);

        mesh->reserve(3, 1, ::fwData::Mesh::CellType::TRIANGLE, ::fwData::Mesh::Attributes::POINT_COLORS);

        ::fwData::Mesh::PointValueType A[3] = {0., 0., 0. };
        ::fwData::Mesh::PointValueType B[3] = {1., 0., 0. };
        ::fwData::Mesh::PointValueType C[3] = {1., 1., 0. };

        ::fwData::Mesh::Id ids[3];

        ids[0] = mesh->pushPoint(A);
        ids[1] = mesh->pushPoint(B);
        ids[2] = mesh->pushPoint(C);

        CPPUNIT_ASSERT(mesh == sharedInput.get_shared());
        // check if the image is properly locked for dump
        CPPUNIT_ASSERT_NO_THROW(mesh->pushPoint(A));
        CPPUNIT_ASSERT_NO_THROW(mesh->pushPoint(B));
        CPPUNIT_ASSERT_NO_THROW(mesh->pushPoint(C));

        CPPUNIT_ASSERT_NO_THROW(mesh->pushCell(::fwData::Mesh::CellType::TRIANGLE, ids, 3));

        const std::array< ::fwData::Mesh::ColorValueType, 4>  color = {255, 0, 0, 255};

        // This are not locked since they didn't exists when creating the mesh the first time.
        CPPUNIT_ASSERT_NO_THROW(mesh->setPointColor(ids[0], color));
        CPPUNIT_ASSERT_NO_THROW(mesh->setPointColor(ids[1], color));
        CPPUNIT_ASSERT_NO_THROW(mesh->setPointColor(ids[2], color));
    }
    CPPUNIT_ASSERT_THROW(mesh->pushPoint(0.f, 0.f, 0.f), ::fwData::Exception);
}

//------------------------------------------------------------------------------

void LockTest::testThreadedLock()
{
    // Add the service
    ::fwServices::ut::LockedService::sptr lockedService = ::fwServices::ut::LockedService::dynamicCast(::fwServices::add(
                                                                                                           "::fwServices::ut::LockedService"));
    CPPUNIT_ASSERT(lockedService);

    // Create the data
    ::fwData::Integer::csptr input = ::fwData::Integer::New(0);
    ::fwData::Integer::sptr inout  = ::fwData::Integer::New(0);
    ::fwData::Integer::sptr output = ::fwData::Integer::New(0);

    // Register the data
    lockedService->registerInput(input, ::fwServices::ut::LockedService::s_INPUT);
    lockedService->registerInOut(inout, ::fwServices::ut::LockedService::s_INOUT);
    lockedService->setOutput(::fwServices::ut::LockedService::s_OUTPUT, output);
    CPPUNIT_ASSERT_EQUAL(true, lockedService->hasAllRequiredObjects());

    // Test that inputLock doesn't block other reader
    {
        auto weakInput = lockedService->getWeakInput< const ::fwData::Integer >(
            ::fwServices::ut::LockedService::s_INPUT);
        auto sharedInput = weakInput.lock();
        CPPUNIT_ASSERT_EQUAL(input, sharedInput.get_shared());

        std::thread t1(&::fwServices::ut::LockedService::starting, lockedService);
        std::this_thread::sleep_for(std::chrono::milliseconds(500));

        // t1 should be in the sleep_for, so m_started and m_input should still be the initial value
        CPPUNIT_ASSERT_EQUAL(std::int64_t(-1), lockedService->m_input.load());
        CPPUNIT_ASSERT_EQUAL(false, lockedService->m_started.load());

        // Wait for t1 execution (1s)
        t1.join();

        CPPUNIT_ASSERT_EQUAL(std::int64_t(0), lockedService->m_input.load());
        CPPUNIT_ASSERT_EQUAL(true, lockedService->m_started.load());
    }

    // Test that outputLock is blocking
    {
        // Start thread immediatly
        std::thread t2(&::fwServices::ut::LockedService::stopping, lockedService);
        std::this_thread::sleep_for(std::chrono::milliseconds(500));

        // t2 should be in the sleep_for, so m_stopped should still be the initial value
        CPPUNIT_ASSERT_EQUAL(false, lockedService->m_stopped.load());

        {
            // We should be blocked here, as long as t2 is alive
            auto weakOutput = lockedService->getWeakOutput< ::fwData::Integer >(
                ::fwServices::ut::LockedService::s_OUTPUT);
            auto sharedOutput = weakOutput.lock();

            // Once t2 have finished, we should be able to overwrite output
            sharedOutput->setValue(666);
        }

        t2.join();

        CPPUNIT_ASSERT_EQUAL(true, lockedService->m_stopped.load());
        CPPUNIT_ASSERT_EQUAL(std::int64_t(666), output->getValue());
    }

    // cleanup
    ::fwServices::OSR::unregisterService(lockedService);
}

} //namespace ut
} //namespace fwServices
