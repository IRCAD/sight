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

#include "LockTest.hpp"

#include <core/thread/Worker.hpp>

#include <data/Image.hpp>
#include <data/Integer.hpp>
#include <data/Mesh.hpp>
#include <data/mt/locked_ptr.hpp>
#include <data/mt/weak_ptr.hpp>

#include <service/IService.hpp>
#include <service/macros.hpp>
#include <service/op/Add.hpp>
#include <service/op/Get.hpp>

#include <utestData/generator/Image.hpp>
#include <utestData/generator/Mesh.hpp>

#include <atomic>
#include <chrono>
#include <thread>

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION( ::sight::service::ut::LockTest );

//------------------------------------------------------------------------------

namespace sight::service
{
namespace ut
{

class LockedService : public service::IService
{
public:

    static const service::IService::KeyType s_INPUT;
    static const service::IService::KeyType s_INOUT;
    static const service::IService::KeyType s_OUTPUT;

    SIGHT_DECLARE_SERVICE(LockedService, service::IService)

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
        auto weakInput   = this->getWeakInput< data::Integer >(s_INPUT);
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
        auto weakOutput   = this->getWeakOutput< data::Integer >(s_OUTPUT);
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

const service::IService::KeyType LockedService::s_INPUT  = "input";
const service::IService::KeyType LockedService::s_INOUT  = "inout";
const service::IService::KeyType LockedService::s_OUTPUT = "output";

SIGHT_REGISTER_SERVICE( ::sight::service::IService, ::sight::service::ut::LockedService)

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

    const auto& services = service::OSR::getServices< service::IService >();
    for (auto service: services)
    {
        if (service->isStarted())
        {
            service->stop();
        }

        service::OSR::unregisterService(service);
    }
}

//------------------------------------------------------------------------------

void LockTest::testScopedLock()
{
    // Add the service
    service::IService::sptr lockedService = service::add("::sight::service::ut::LockedService");
    CPPUNIT_ASSERT(lockedService);

    // Create the data
    data::Integer::csptr input = data::Integer::New(0);
    data::Integer::sptr inout  = data::Integer::New(0);
    data::Integer::sptr output = data::Integer::New(0);

    // Register the data
    lockedService->registerInput(input, service::ut::LockedService::s_INPUT);
    lockedService->registerInOut(inout, service::ut::LockedService::s_INOUT);
    lockedService->setOutput(service::ut::LockedService::s_OUTPUT, output);
    CPPUNIT_ASSERT_EQUAL(true, lockedService->hasAllRequiredObjects());

    // Test basic scoped lock
    data::mt::weak_ptr< const data::Integer > weakInput(input);
    {
        auto sharedInput = weakInput.lock();
        CPPUNIT_ASSERT_EQUAL(input, sharedInput.get_shared());
    }

    data::mt::weak_ptr< data::Integer > weakInOut(inout);
    {
        auto sharedInOut = weakInOut.lock();
        CPPUNIT_ASSERT_EQUAL(inout, sharedInOut.get_shared());
    }

    data::mt::weak_ptr< data::Integer > weakOutput(output);
    {
        auto sharedOutput = weakOutput.lock();
        CPPUNIT_ASSERT_EQUAL(output, sharedOutput.get_shared());
    }

    // Test basic scoped lock from service getters
    weakInput = lockedService->getWeakInput< data::Integer >(service::ut::LockedService::s_INPUT);
    {
        auto sharedInput = weakInput.lock();
        CPPUNIT_ASSERT_EQUAL(input, sharedInput.get_shared());
    }

    weakInOut = lockedService->getWeakInOut< data::Integer >(service::ut::LockedService::s_INOUT);
    {
        auto sharedInOut = weakInOut.lock();
        CPPUNIT_ASSERT_EQUAL(inout, sharedInOut.get_shared());
    }

    weakOutput = lockedService->getWeakOutput< data::Integer >(service::ut::LockedService::s_OUTPUT);
    {
        auto sharedOutput = weakOutput.lock();
        CPPUNIT_ASSERT_EQUAL(output, sharedOutput.get_shared());
    }

    // Test basic scoped lock from service direct locker
    {
        auto sharedInput = lockedService->getLockedInput< data::Integer >(service::ut::LockedService::s_INPUT);
        CPPUNIT_ASSERT_EQUAL(input, sharedInput.get_shared());
    }

    {
        auto sharedInOut = lockedService->getLockedInOut< data::Integer >(service::ut::LockedService::s_INOUT);
        CPPUNIT_ASSERT_EQUAL(inout, sharedInOut.get_shared());
    }

    {
        auto sharedOutput = lockedService->getLockedOutput< data::Integer >(
            service::ut::LockedService::s_OUTPUT);
        CPPUNIT_ASSERT_EQUAL(output, sharedOutput.get_shared());
    }

    // cleanup
    service::OSR::unregisterService(lockedService);
}

//------------------------------------------------------------------------------

void LockTest::testDumpLock()
{
    data::Image::sptr image = data::Image::New();

    utestData::generator::Image::generateRandomImage(image, core::tools::Type::s_UINT8);

    // Add the service
    service::IService::sptr lockedService = service::add("::sight::service::ut::LockedService");
    CPPUNIT_ASSERT(lockedService);

    lockedService->registerInput(image, service::ut::LockedService::s_INPUT);

    {
        auto sharedInput = lockedService->getLockedInput< data::Image >(service::ut::LockedService::s_INPUT);
        CPPUNIT_ASSERT(image == sharedInput.get_shared());
        // check if the image is properly locked for dump
        CPPUNIT_ASSERT_NO_THROW(image->getBuffer());
    }

    bool exceptionReceived = false;

    for(int i = 3; --i > 0 && !exceptionReceived;)
    {
        try
        {
            image->getBuffer();
        }
        catch(data::Exception&)
        {
            exceptionReceived = true;
        }

        if(!exceptionReceived)
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }
    }

    CPPUNIT_ASSERT(exceptionReceived);

    data::Mesh::sptr mesh = data::Mesh::New();

    lockedService->registerInput(mesh, service::ut::LockedService::s_INPUT);

    {
        auto sharedInput = lockedService->getLockedInput< data::Mesh >(service::ut::LockedService::s_INPUT);

        mesh->reserve(3, 1, data::Mesh::CellType::TRIANGLE, data::Mesh::Attributes::POINT_COLORS);

        data::Mesh::PointValueType A[3] = {0., 0., 0. };
        data::Mesh::PointValueType B[3] = {1., 0., 0. };
        data::Mesh::PointValueType C[3] = {1., 1., 0. };

        data::Mesh::PointId ids[3];

        ids[0] = mesh->pushPoint(A);
        ids[1] = mesh->pushPoint(B);
        ids[2] = mesh->pushPoint(C);

        CPPUNIT_ASSERT(mesh == sharedInput.get_shared());
        // check if the image is properly locked for dump
        CPPUNIT_ASSERT_NO_THROW(mesh->pushPoint(A));
        CPPUNIT_ASSERT_NO_THROW(mesh->pushPoint(B));
        CPPUNIT_ASSERT_NO_THROW(mesh->pushPoint(C));

        CPPUNIT_ASSERT_NO_THROW(mesh->pushCell(data::Mesh::CellType::TRIANGLE, ids, 3));

        const std::array< data::Mesh::ColorValueType, 4>  color = {255, 0, 0, 255};

        // This are not locked since they didn't exists when creating the mesh the first time.
        CPPUNIT_ASSERT_NO_THROW(mesh->setPointColor(ids[0], color));
        CPPUNIT_ASSERT_NO_THROW(mesh->setPointColor(ids[1], color));
        CPPUNIT_ASSERT_NO_THROW(mesh->setPointColor(ids[2], color));
    }

    exceptionReceived = false;

    for(int i = 3; --i > 0 && !exceptionReceived;)
    {
        try
        {
            mesh->pushPoint(0.f, 0.f, 0.f);
        }
        catch(data::Exception&)
        {
            exceptionReceived = true;
        }

        if(!exceptionReceived)
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }
    }

    CPPUNIT_ASSERT(exceptionReceived);
}

//------------------------------------------------------------------------------

void LockTest::testThreadedLock()
{
    // Add the service
    service::ut::LockedService::sptr lockedService = service::ut::LockedService::dynamicCast(service::add(
                                                                                                 "::sight::service::ut::LockedService"));
    CPPUNIT_ASSERT(lockedService);

    // Create the data
    data::Integer::csptr input = data::Integer::New(0);
    data::Integer::sptr inout  = data::Integer::New(0);
    data::Integer::sptr output = data::Integer::New(0);

    // Register the data
    lockedService->registerInput(input, service::ut::LockedService::s_INPUT);
    lockedService->registerInOut(inout, service::ut::LockedService::s_INOUT);
    lockedService->setOutput(service::ut::LockedService::s_OUTPUT, output);
    CPPUNIT_ASSERT_EQUAL(true, lockedService->hasAllRequiredObjects());

    // Test that inputLock doesn't block other reader
    {
        auto weakInput = lockedService->getWeakInput< const data::Integer >(
            service::ut::LockedService::s_INPUT);
        auto sharedInput = weakInput.lock();
        CPPUNIT_ASSERT_EQUAL(input, sharedInput.get_shared());

        std::thread t1(&::sight::service::ut::LockedService::starting, lockedService);
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
        std::thread t2(&::sight::service::ut::LockedService::stopping, lockedService);
        std::this_thread::sleep_for(std::chrono::milliseconds(500));

        // t2 should be in the sleep_for, so m_stopped should still be the initial value
        CPPUNIT_ASSERT_EQUAL(false, lockedService->m_stopped.load());

        {
            // We should be blocked here, as long as t2 is alive
            auto weakOutput = lockedService->getWeakOutput< data::Integer >(
                service::ut::LockedService::s_OUTPUT);
            auto sharedOutput = weakOutput.lock();

            // Once t2 have finished, we should be able to overwrite output
            sharedOutput->setValue(666);
        }

        t2.join();

        CPPUNIT_ASSERT_EQUAL(true, lockedService->m_stopped.load());
        CPPUNIT_ASSERT_EQUAL(std::int64_t(666), output->getValue());
    }

    // cleanup
    service::OSR::unregisterService(lockedService);
}

} //namespace ut
} //namespace sight::service
