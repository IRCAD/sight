/************************************************************************
 *
 * Copyright (C) 2009-2024 IRCAD France
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

#include "lock_test.hpp"

#include <core/thread/worker.hpp>

#include <data/image.hpp>
#include <data/integer.hpp>
#include <data/mesh.hpp>
#include <data/mt/locked_ptr.hpp>
#include <data/mt/weak_ptr.hpp>

#include <service/base.hpp>
#include <service/macros.hpp>
#include <service/op.hpp>

#include <utest_data/generator/image.hpp>
#include <utest_data/generator/mesh.hpp>

#include <atomic>
#include <chrono>
#include <thread>

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION(sight::service::ut::lock_test);

//------------------------------------------------------------------------------

namespace sight::service::ut
{

class locked_service : public service::base
{
public:

    static const service::base::key_t INPUT;
    static const service::base::key_t INOUT;
    static const service::base::key_t OUTPUT;

    SIGHT_DECLARE_SERVICE(locked_service, service::base);

    locked_service() noexcept =
        default;

    std::atomic_bool m_started {false};
    std::atomic_bool m_stopped {false};
    std::atomic_int64_t m_input_value {-1};

    //------------------------------------------------------------------------------

    void starting() final
    {
        // Reading should not be blocked by other reader
        auto input = m_input.lock();

        // Simulate working....
        std::this_thread::sleep_for(std::chrono::seconds(2));

        m_input_value = std::dynamic_pointer_cast<const sight::data::integer>(input.get_shared())->get_value();
        m_started     = true;
    }

    //------------------------------------------------------------------------------

    void stopping() final
    {
        // Reading should not be blocked by other reader
        auto shared_output = m_output.lock();

        shared_output->set_value(-1);

        // Simulate working....
        std::this_thread::sleep_for(std::chrono::seconds(2));

        shared_output->set_value(1);
        m_stopped = true;
    }

    //------------------------------------------------------------------------------

    void configuring() final
    {
    }

    //------------------------------------------------------------------------------

    void updating() final
    {
    }

    data::ptr<data::object, data::access::in> m_input {this, INPUT};
    data::ptr<data::integer, data::access::inout> m_inout {this, INOUT};
    data::ptr<data::integer, data::access::out> m_output {this, OUTPUT, true};
};

const service::base::key_t locked_service::INPUT  = "input";
const service::base::key_t locked_service::INOUT  = "inout";
const service::base::key_t locked_service::OUTPUT = "output";

SIGHT_REGISTER_SERVICE(sight::service::base, sight::service::ut::locked_service);

//------------------------------------------------------------------------------

void lock_test::setUp()
{
    // Set up context before running a test.
}

//------------------------------------------------------------------------------

void lock_test::tearDown()
{
    // Clean up after the test run.
    // unregister the services that have not been unregistered because a test failed.

    const auto& services = sight::service::get_services<service::base>();
    for(const auto& service : services)
    {
        if(service->started())
        {
            service->stop();
        }

        service::unregister_service(service);
    }
}

//------------------------------------------------------------------------------

void lock_test::test_scoped_lock()
{
    // Add the service
    service::base::sptr locked_service = service::add("sight::service::ut::locked_service");
    CPPUNIT_ASSERT(locked_service);

    // Create the data
    data::integer::csptr input = std::make_shared<data::integer>(0);
    data::integer::sptr inout  = std::make_shared<data::integer>(0);
    data::integer::sptr output = std::make_shared<data::integer>(0);

    CPPUNIT_ASSERT_EQUAL(std::uint64_t(0), inout->last_modified());
    CPPUNIT_ASSERT_EQUAL(std::uint64_t(0), output->last_modified());

    // Register the data
    locked_service->set_input(input, service::ut::locked_service::INPUT);
    locked_service->set_inout(inout, service::ut::locked_service::INOUT);
    locked_service->set_output(output, service::ut::locked_service::OUTPUT);

    // Test basic scoped lock
    data::mt::weak_ptr<const data::integer> weak_input(input);
    {
        auto shared_input = weak_input.lock();
        CPPUNIT_ASSERT_EQUAL(input, shared_input.get_shared());
    }

    data::mt::weak_ptr<data::integer> weak_in_out(inout);
    {
        auto shared_in_out = weak_in_out.lock();
        CPPUNIT_ASSERT_EQUAL(inout, shared_in_out.get_shared());
        CPPUNIT_ASSERT_EQUAL(std::uint64_t(1), shared_in_out->last_modified());
    }

    data::mt::weak_ptr<data::integer> weak_output(output);
    {
        auto shared_output = weak_output.lock();
        CPPUNIT_ASSERT_EQUAL(output, shared_output.get_shared());
        CPPUNIT_ASSERT_EQUAL(std::uint64_t(1), shared_output->last_modified());
    }

    // Test basic scoped lock from service getters
    weak_input = locked_service->input<data::integer>(service::ut::locked_service::INPUT);
    {
        auto shared_input = weak_input.lock();
        CPPUNIT_ASSERT_EQUAL(input, shared_input.get_shared());
    }

    weak_in_out = locked_service->inout<data::integer>(service::ut::locked_service::INOUT);
    {
        auto shared_in_out = weak_in_out.lock();
        CPPUNIT_ASSERT_EQUAL(inout, shared_in_out.get_shared());
        CPPUNIT_ASSERT_EQUAL(std::uint64_t(2), shared_in_out->last_modified());
    }

    weak_output = locked_service->output<data::integer>(service::ut::locked_service::OUTPUT);
    {
        auto shared_output = weak_output.lock();
        CPPUNIT_ASSERT_EQUAL(output, shared_output.get_shared());
        CPPUNIT_ASSERT_EQUAL(std::uint64_t(2), shared_output->last_modified());
    }

    // Test basic scoped lock from service direct locker
    {
        auto shared_input = locked_service->input<data::integer>(service::ut::locked_service::INPUT);
        CPPUNIT_ASSERT(shared_input.lock() == input);
    }

    {
        auto shared_in_out = locked_service->inout<data::integer>(service::ut::locked_service::INOUT);
        CPPUNIT_ASSERT(shared_in_out.lock() == inout);
    }

    {
        auto shared_output = locked_service->output<data::integer>(
            service::ut::locked_service::OUTPUT
        );
        CPPUNIT_ASSERT(shared_output.lock() == output);
    }

    // cleanup
    service::unregister_service(locked_service);
}

//------------------------------------------------------------------------------

void lock_test::test_dump_lock()
{
    data::image::sptr image = std::make_shared<data::image>();

    utest_data::generator::image::generate_random_image(image, core::type::UINT8);

    // Add the service
    service::base::sptr locked_service = service::add("sight::service::ut::locked_service");
    CPPUNIT_ASSERT(locked_service);

    locked_service->set_input(image, service::ut::locked_service::INPUT);

    {
        auto shared_input = locked_service->input<data::image>(service::ut::locked_service::INPUT).lock();
        CPPUNIT_ASSERT(shared_input == image);
        // check if the image is properly locked for dump
        CPPUNIT_ASSERT_NO_THROW(image->buffer());
    }

    bool exception_received = false;

    for(int i = 3 ; --i > 0 && !exception_received ; )
    {
        try
        {
            image->buffer();
        }
        catch(data::exception&)
        {
            exception_received = true;
        }

        if(!exception_received)
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }
    }

    CPPUNIT_ASSERT(exception_received);

    data::mesh::sptr mesh = std::make_shared<data::mesh>();

    locked_service->set_input(mesh, service::ut::locked_service::INPUT);

    {
        auto shared_input = locked_service->input<data::mesh>(service::ut::locked_service::INPUT).lock();

        mesh->reserve(3, 1, data::mesh::cell_type_t::triangle, data::mesh::attribute::point_colors);

        const std::array<data::mesh::position_t, 3> a = {0., 0., 0.};
        const std::array<data::mesh::position_t, 3> b = {1., 0., 0.};
        const std::array<data::mesh::position_t, 3> c = {1., 1., 0.};

        std::array<data::mesh::point_t, 3> ids {};

        ids[0] = mesh->push_point(a);
        ids[1] = mesh->push_point(b);
        ids[2] = mesh->push_point(c);

        CPPUNIT_ASSERT(shared_input == mesh);
        // check if the image is properly locked for dump
        CPPUNIT_ASSERT_NO_THROW(mesh->push_point(a));
        CPPUNIT_ASSERT_NO_THROW(mesh->push_point(b));
        CPPUNIT_ASSERT_NO_THROW(mesh->push_point(c));

        CPPUNIT_ASSERT_NO_THROW(mesh->push_cell(std::vector(ids.begin(), ids.end())));

        const std::array<data::mesh::color_t, 4> color = {255, 0, 0, 255};

        // This are not locked since they didn't exists when creating the mesh the first time.
        CPPUNIT_ASSERT_NO_THROW(mesh->set_point_color(ids[0], color));
        CPPUNIT_ASSERT_NO_THROW(mesh->set_point_color(ids[1], color));
        CPPUNIT_ASSERT_NO_THROW(mesh->set_point_color(ids[2], color));
    }

    exception_received = false;

    for(int i = 3 ; --i > 0 && !exception_received ; )
    {
        try
        {
            mesh->push_point(0.F, 0.F, 0.F);
        }
        catch(data::exception&)
        {
            exception_received = true;
        }

        if(!exception_received)
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }
    }

    CPPUNIT_ASSERT(exception_received);
}

//------------------------------------------------------------------------------

void lock_test::test_threaded_lock()
{
    // Add the service
    auto locked_service = service::add<service::ut::locked_service>("sight::service::ut::locked_service");
    CPPUNIT_ASSERT(locked_service);

    // Create the data
    data::integer::csptr input = std::make_shared<data::integer>(0);
    data::integer::sptr inout  = std::make_shared<data::integer>(0);
    data::integer::sptr output = std::make_shared<data::integer>(0);

    // Register the data
    locked_service->set_input(input, service::ut::locked_service::INPUT);
    locked_service->set_inout(inout, service::ut::locked_service::INOUT);
    locked_service->set_output(output, service::ut::locked_service::OUTPUT);

    // Test that inputLock doesn't block other reader
    {
        auto weak_input = locked_service->input<const data::integer>(
            service::ut::locked_service::INPUT
        );
        auto shared_input = weak_input.lock();
        CPPUNIT_ASSERT_EQUAL(input, shared_input.get_shared());

        std::thread t1(&sight::service::ut::locked_service::starting, locked_service);
        std::this_thread::sleep_for(std::chrono::milliseconds(500));

        // t1 should be in the sleep_for, so m_started and m_input should still be the initial value
        CPPUNIT_ASSERT_EQUAL(std::int64_t(-1), locked_service->m_input_value.load());
        CPPUNIT_ASSERT_EQUAL(false, locked_service->m_started.load());

        // Wait for t1 execution (1s)
        t1.join();

        CPPUNIT_ASSERT_EQUAL(std::int64_t(0), locked_service->m_input_value.load());
        CPPUNIT_ASSERT_EQUAL(true, locked_service->m_started.load());
    }

    // Test that outputLock is blocking
    {
        // Start thread immediately
        std::thread t2(&sight::service::ut::locked_service::stopping, locked_service);
        std::this_thread::sleep_for(std::chrono::milliseconds(500));

        // t2 should be in the sleep_for, so m_stopped should still be the initial value
        CPPUNIT_ASSERT_EQUAL(false, locked_service->m_stopped.load());

        {
            // We should be blocked here, as long as t2 is alive
            auto weak_output = locked_service->output<data::integer>(
                service::ut::locked_service::OUTPUT
            );
            auto shared_output = weak_output.lock();

            // Once t2 have finished, we should be able to overwrite output
            shared_output->set_value(666);
        }

        t2.join();

        CPPUNIT_ASSERT_EQUAL(true, locked_service->m_stopped.load());
        CPPUNIT_ASSERT_EQUAL(std::int64_t(666), output->get_value());
    }

    // cleanup
    service::unregister_service(locked_service);
}

} // namespace sight::service::ut
