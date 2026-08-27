/************************************************************************
 *
 * Copyright (C) 2009-2026 IRCAD France
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

#include <data/image.hpp>
#include <data/integer.hpp>
#include <data/mesh.hpp>
#include <data/mt/locked_ptr.hpp>
#include <data/mt/weak_ptr.hpp>

#include <service/base.hpp>
#include <service/macros.hpp>
#include <service/op.hpp>

#include <utest/service_fixture.hpp>
#include <utest_data/generator/image.hpp>

#include <doctest/doctest.h>

#include <atomic>
#include <condition_variable>
#include <future>
#include <mutex>
#include <thread>

//------------------------------------------------------------------------------

namespace
{

class locked_srv : public sight::service::base
{
public:

    static const sight::service::base::key_t INPUT;
    static const sight::service::base::key_t INOUT;
    static const sight::service::base::key_t OUTPUT;

    SIGHT_DECLARE_SERVICE(locked_srv, sight::service::base);

    // NOLINTBEGIN(cppcoreguidelines-non-private-member-variables-in-classes)
    std::atomic_bool m_started {false};
    std::atomic_bool m_stopped {false};
    std::atomic_int64_t m_input_value {-1};
    // NOLINTEND(cppcoreguidelines-non-private-member-variables-in-classes)

    //------------------------------------------------------------------------------

    void wait_for_input_lock()
    {
        std::unique_lock lock(m_sync_mutex);
        m_sync_condition.wait(lock, [this]{return m_input_locked;});
    }

    //------------------------------------------------------------------------------

    void release_input_lock()
    {
        {
            std::scoped_lock lock(m_sync_mutex);
            m_release_input = true;
        }
        m_sync_condition.notify_all();
    }

    //------------------------------------------------------------------------------

    void wait_for_output_lock()
    {
        std::unique_lock lock(m_sync_mutex);
        m_sync_condition.wait(lock, [this]{return m_output_locked;});
    }

    //------------------------------------------------------------------------------

    void release_output_lock()
    {
        {
            std::scoped_lock lock(m_sync_mutex);
            m_release_output = true;
        }
        m_sync_condition.notify_all();
    }

protected:

    //------------------------------------------------------------------------------

    void starting() final
    {
        // Reading should not be blocked by other reader
        auto input = m_input.lock();

        {
            std::scoped_lock lock(m_sync_mutex);
            m_input_locked = true;
        }
        m_sync_condition.notify_all();

        std::unique_lock lock(m_sync_mutex);
        m_sync_condition.wait(lock, [this]{return m_release_input;});

        m_input_value = std::dynamic_pointer_cast<const sight::data::integer>(input.get_shared())->get_value();
        m_started     = true;
    }

    //------------------------------------------------------------------------------

    void stopping() final
    {
        // Reading should not be blocked by other reader
        auto shared_output = m_output.lock();

        shared_output->set_value(-1);

        {
            std::scoped_lock lock(m_sync_mutex);
            m_output_locked = true;
        }
        m_sync_condition.notify_all();

        std::unique_lock lock(m_sync_mutex);
        m_sync_condition.wait(lock, [this]{return m_release_output;});

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

private:

    sight::data::ptr<sight::data::object, sight::data::access::in> m_input {this, INPUT};
    sight::data::ptr<sight::data::integer, sight::data::access::inout> m_inout {this, INOUT};
    sight::data::ptr<sight::data::integer, sight::data::access::out> m_output {this, OUTPUT};

    std::mutex m_sync_mutex;
    std::condition_variable m_sync_condition;
    bool m_input_locked {false};
    bool m_output_locked {false};
    bool m_release_input {false};
    bool m_release_output {false};
};

const sight::service::base::key_t locked_srv::INPUT  = "input";
const sight::service::base::key_t locked_srv::INOUT  = "inout";
const sight::service::base::key_t locked_srv::OUTPUT = "output";

SIGHT_REGISTER_SERVICE(sight::service::base, locked_srv);

struct fixture
{
    ~fixture()
    {
        if(m_locked_srv && m_locked_srv->started())
        {
            m_locked_srv->stop();
        }

        if(m_locked_srv)
        {
            sight::service::remove(m_locked_srv);
        }
    }

    locked_srv::sptr m_locked_srv;
};

} // namespace

TEST_SUITE("sight::service::lock")
{
//------------------------------------------------------------------------------

    TEST_CASE_FIXTURE(fixture, "test_scoped_lock")
    {
        // Add the service
        m_locked_srv = sight::service::add<locked_srv>("locked_srv");
        CHECK(m_locked_srv);

        // Create the data
        sight::data::integer::csptr input = std::make_shared<sight::data::integer>(0);
        sight::data::integer::sptr inout  = std::make_shared<sight::data::integer>(0);
        sight::data::integer::sptr output = std::make_shared<sight::data::integer>(0);

        CHECK_EQ(std::uint64_t(0), inout->last_modified());
        CHECK_EQ(std::uint64_t(0), output->last_modified());

        // Register the data
        m_locked_srv->set_input(input, locked_srv::INPUT);
        m_locked_srv->set_inout(inout, locked_srv::INOUT);
        m_locked_srv->set_output(output, locked_srv::OUTPUT);

        // Test basic scoped lock
        sight::data::mt::weak_ptr<const sight::data::integer> weak_input(input);
        {
            auto shared_input = weak_input.lock();
            CHECK_EQ(input, shared_input.get_shared());
        }

        sight::data::mt::weak_ptr<sight::data::integer> weak_in_out(inout);
        {
            auto shared_in_out = weak_in_out.lock();
            CHECK_EQ(inout, shared_in_out.get_shared());
            CHECK_EQ(std::uint64_t(1), shared_in_out->last_modified());
        }

        sight::data::mt::weak_ptr<sight::data::integer> weak_output(output);
        {
            auto shared_output = weak_output.lock();
            CHECK_EQ(output, shared_output.get_shared());
            CHECK_EQ(std::uint64_t(1), shared_output->last_modified());
        }

        // Test basic scoped lock from service getters
        weak_input = m_locked_srv->input<sight::data::integer>(locked_srv::INPUT);
        {
            auto shared_input = weak_input.lock();
            CHECK_EQ(input, shared_input.get_shared());
        }

        weak_in_out = m_locked_srv->inout<sight::data::integer>(locked_srv::INOUT);
        {
            auto shared_in_out = weak_in_out.lock();
            CHECK_EQ(inout, shared_in_out.get_shared());
            CHECK_EQ(std::uint64_t(2), shared_in_out->last_modified());
        }

        weak_output = m_locked_srv->output<sight::data::integer>(locked_srv::OUTPUT);
        {
            auto shared_output = weak_output.lock();
            CHECK_EQ(output, shared_output.get_shared());
            CHECK_EQ(std::uint64_t(2), shared_output->last_modified());
        }

        // Test basic scoped lock from service direct locker
        {
            auto shared_input =
                m_locked_srv->input<sight::data::integer>(locked_srv::INPUT);
            CHECK(shared_input.lock() == input);
        }

        {
            auto shared_in_out =
                m_locked_srv->inout<sight::data::integer>(locked_srv::INOUT);
            CHECK(shared_in_out.lock() == inout);
        }

        {
            auto shared_output = m_locked_srv->output<sight::data::integer>(
                locked_srv::OUTPUT
            );
            CHECK(shared_output.lock() == output);
        }
    }

//------------------------------------------------------------------------------

    TEST_CASE_FIXTURE(fixture, "test_dump_lock")
    {
        sight::data::image::sptr image = std::make_shared<sight::data::image>();

        sight::utest_data::generator::image::generate_image(image);

        // Add the service
        m_locked_srv = sight::service::add<locked_srv>("locked_srv");
        CHECK(m_locked_srv);

        m_locked_srv->set_input(image, locked_srv::INPUT);

        {
            auto shared_input =
                m_locked_srv->input<sight::data::image>(locked_srv::INPUT).lock();
            CHECK(shared_input == image);
            // check if the image is properly locked for dump
            CHECK_NOTHROW(image->buffer());
        }

        CHECK_THROWS_AS(image->buffer(), sight::data::exception);

        sight::data::mesh::sptr mesh = std::make_shared<sight::data::mesh>();

        m_locked_srv->set_input(mesh, locked_srv::INPUT);

        {
            auto shared_input = m_locked_srv->input<sight::data::mesh>(locked_srv::INPUT).lock();

            mesh->reserve(3, 1, sight::data::mesh::cell_type_t::triangle, sight::data::mesh::attribute::point_colors);

            const std::array<sight::data::mesh::position_t, 3> a = {0., 0., 0.};
            const std::array<sight::data::mesh::position_t, 3> b = {1., 0., 0.};
            const std::array<sight::data::mesh::position_t, 3> c = {1., 1., 0.};

            std::array<sight::data::mesh::point_t, 3> ids {};

            ids[0] = mesh->push_point(a);
            ids[1] = mesh->push_point(b);
            ids[2] = mesh->push_point(c);

            CHECK(shared_input == mesh);
            // check if the image is properly locked for dump
            CHECK_NOTHROW(mesh->push_point(a));
            CHECK_NOTHROW(mesh->push_point(b));
            CHECK_NOTHROW(mesh->push_point(c));

            CHECK_NOTHROW(mesh->push_cell(std::vector(ids.begin(), ids.end())));

            const std::array<sight::data::mesh::color_t, 4> color = {255, 0, 0, 255};

            // This are not locked since they didn't exists when creating the mesh the first time.
            CHECK_NOTHROW(mesh->set_point_color(ids[0], color));
            CHECK_NOTHROW(mesh->set_point_color(ids[1], color));
            CHECK_NOTHROW(mesh->set_point_color(ids[2], color));
        }

        CHECK_THROWS_AS(mesh->push_point(0.F, 0.F, 0.F), sight::data::exception);
    }

//------------------------------------------------------------------------------

    TEST_CASE_FIXTURE(fixture, "test_threaded_lock")
    {
        // Add the service
        m_locked_srv = sight::service::add<locked_srv>("locked_srv");
        CHECK(m_locked_srv);

        // Create the data
        sight::data::integer::csptr input = std::make_shared<sight::data::integer>(0);
        sight::data::integer::sptr inout  = std::make_shared<sight::data::integer>(0);
        sight::data::integer::sptr output = std::make_shared<sight::data::integer>(0);

        // Register the data
        m_locked_srv->set_input(input, locked_srv::INPUT);
        m_locked_srv->set_inout(inout, locked_srv::INOUT);
        m_locked_srv->set_output(output, locked_srv::OUTPUT);

        const auto worker = sight::core::thread::worker::make();
        m_locked_srv->set_worker(worker);

        // Test that inputLock doesn't block other reader
        {
            auto weak_input   = m_locked_srv->input<const sight::data::integer>(locked_srv::INPUT);
            auto shared_input = weak_input.lock();
            CHECK_EQ(input, shared_input.get_shared());

            auto start_future = m_locked_srv->start();
            m_locked_srv->wait_for_input_lock();

            // t1 holds a read lock, but has not been allowed to finish yet.
            CHECK_EQ(std::int64_t(-1), m_locked_srv->m_input_value.load());
            CHECK_EQ(false, m_locked_srv->m_started.load());

            m_locked_srv->release_input_lock();
            start_future.get();

            CHECK_EQ(std::int64_t(0), m_locked_srv->m_input_value.load());
            CHECK_EQ(true, m_locked_srv->m_started.load());
        }

        // Test that outputLock is blocking
        {
            auto stop_future = m_locked_srv->stop();
            m_locked_srv->wait_for_output_lock();

            // t2 holds the write lock and has not been allowed to finish yet.
            CHECK_EQ(false, m_locked_srv->m_stopped.load());

            std::promise<void> output_lock_attempted;
            std::atomic_bool output_lock_completed {false};
            std::thread t3(
                [&]
                {
                output_lock_attempted.set_value();
                auto shared_output = m_locked_srv->output<sight::data::integer>(locked_srv::OUTPUT).lock();
                shared_output->set_value(666);
                output_lock_completed = true;
                });

            // t3 has started trying to acquire the lock, which t2 still holds.
            output_lock_attempted.get_future().wait();
            CHECK_FALSE(output_lock_completed.load());

            m_locked_srv->release_output_lock();

            stop_future.get();
            t3.join();

            CHECK_EQ(true, m_locked_srv->m_stopped.load());
            CHECK_EQ(std::int64_t(666), output->get_value());
        }
        worker->stop();
    }
} // end TEST_SUITE
