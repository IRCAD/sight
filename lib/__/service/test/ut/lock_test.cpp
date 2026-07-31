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

#include <utest_data/generator/image.hpp>
#include <utest_data/generator/mesh.hpp>

#include <doctest/doctest.h>

#include <atomic>
#include <thread>

//------------------------------------------------------------------------------

namespace sight::service::ut
{

class locked_srv : public sight::service::base
{
public:

    static const sight::service::base::key_t INPUT;
    static const sight::service::base::key_t INOUT;
    static const sight::service::base::key_t OUTPUT;

    SIGHT_DECLARE_SERVICE(locked_srv, sight::service::base);

    locked_srv() noexcept =
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

    sight::data::ptr<sight::data::object, sight::data::access::in> m_input {this, INPUT};
    sight::data::ptr<sight::data::integer, sight::data::access::inout> m_inout {this, INOUT};
    sight::data::ptr<sight::data::integer, sight::data::access::out> m_output {this, OUTPUT};
};

const sight::service::base::key_t locked_srv::INPUT  = "input";
const sight::service::base::key_t locked_srv::INOUT  = "inout";
const sight::service::base::key_t locked_srv::OUTPUT = "output";

SIGHT_REGISTER_SERVICE(sight::service::base, sight::service::ut::locked_srv);

namespace
{

struct fixture
{
    ~fixture()
    {
        if(locked_srv->started())
        {
            locked_srv->stop();
        }

        sight::service::remove(locked_srv);
    }

    sight::service::ut::locked_srv::sptr locked_srv;
};

} // namespace

TEST_SUITE("sight::service::lock")
{
//------------------------------------------------------------------------------

    TEST_CASE_FIXTURE(fixture, "test_scoped_lock")
    {
        // Add the service
        locked_srv = sight::service::add<sight::service::ut::locked_srv>("sight::service::ut::locked_srv");
        CHECK(locked_srv);

        // Create the data
        sight::data::integer::csptr input = std::make_shared<sight::data::integer>(0);
        sight::data::integer::sptr inout  = std::make_shared<sight::data::integer>(0);
        sight::data::integer::sptr output = std::make_shared<sight::data::integer>(0);

        CHECK_EQ(std::uint64_t(0), inout->last_modified());
        CHECK_EQ(std::uint64_t(0), output->last_modified());

        // Register the data
        locked_srv->set_input(input, sight::service::ut::locked_srv::INPUT);
        locked_srv->set_inout(inout, sight::service::ut::locked_srv::INOUT);
        locked_srv->set_output(output, sight::service::ut::locked_srv::OUTPUT);

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
        weak_input = locked_srv->input<sight::data::integer>(sight::service::ut::locked_srv::INPUT);
        {
            auto shared_input = weak_input.lock();
            CHECK_EQ(input, shared_input.get_shared());
        }

        weak_in_out = locked_srv->inout<sight::data::integer>(sight::service::ut::locked_srv::INOUT);
        {
            auto shared_in_out = weak_in_out.lock();
            CHECK_EQ(inout, shared_in_out.get_shared());
            CHECK_EQ(std::uint64_t(2), shared_in_out->last_modified());
        }

        weak_output = locked_srv->output<sight::data::integer>(sight::service::ut::locked_srv::OUTPUT);
        {
            auto shared_output = weak_output.lock();
            CHECK_EQ(output, shared_output.get_shared());
            CHECK_EQ(std::uint64_t(2), shared_output->last_modified());
        }

        // Test basic scoped lock from service direct locker
        {
            auto shared_input =
                locked_srv->input<sight::data::integer>(sight::service::ut::locked_srv::INPUT);
            CHECK(shared_input.lock() == input);
        }

        {
            auto shared_in_out =
                locked_srv->inout<sight::data::integer>(sight::service::ut::locked_srv::INOUT);
            CHECK(shared_in_out.lock() == inout);
        }

        {
            auto shared_output = locked_srv->output<sight::data::integer>(
                sight::service::ut::locked_srv::OUTPUT
            );
            CHECK(shared_output.lock() == output);
        }
    }

//------------------------------------------------------------------------------

    TEST_CASE_FIXTURE(fixture, "test_dump_lock")
    {
        sight::data::image::sptr image = std::make_shared<sight::data::image>();

        sight::utest_data::generator::image::generate_random_image(image, sight::core::type::UINT8);

        // Add the service
        locked_srv = sight::service::add<sight::service::ut::locked_srv>("sight::service::ut::locked_srv");
        CHECK(locked_srv);

        locked_srv->set_input(image, sight::service::ut::locked_srv::INPUT);

        {
            auto shared_input =
                locked_srv->input<sight::data::image>(sight::service::ut::locked_srv::INPUT).lock();
            CHECK(shared_input == image);
            // check if the image is properly locked for dump
            CHECK_NOTHROW(image->buffer());
        }

        bool exception_received = false;

        for(int i = 3 ; --i > 0 && !exception_received ; )
        {
            try
            {
                image->buffer();
            }
            catch(sight::data::exception&)
            {
                exception_received = true;
            }

            if(!exception_received)
            {
                std::this_thread::sleep_for(std::chrono::milliseconds(100));
            }
        }

        CHECK(exception_received);

        sight::data::mesh::sptr mesh = std::make_shared<sight::data::mesh>();

        locked_srv->set_input(mesh, sight::service::ut::locked_srv::INPUT);

        {
            auto shared_input =
                locked_srv->input<sight::data::mesh>(sight::service::ut::locked_srv::INPUT).lock();

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

        exception_received = false;

        for(int i = 3 ; --i > 0 && !exception_received ; )
        {
            try
            {
                mesh->push_point(0.F, 0.F, 0.F);
            }
            catch(sight::data::exception&)
            {
                exception_received = true;
            }

            if(!exception_received)
            {
                std::this_thread::sleep_for(std::chrono::milliseconds(100));
            }
        }

        CHECK(exception_received);
    }

//------------------------------------------------------------------------------

    TEST_CASE_FIXTURE(fixture, "test_threaded_lock")
    {
        // Add the service
        locked_srv = sight::service::add<sight::service::ut::locked_srv>("sight::service::ut::locked_srv");
        CHECK(locked_srv);

        // Create the data
        sight::data::integer::csptr input = std::make_shared<sight::data::integer>(0);
        sight::data::integer::sptr inout  = std::make_shared<sight::data::integer>(0);
        sight::data::integer::sptr output = std::make_shared<sight::data::integer>(0);

        // Register the data
        locked_srv->set_input(input, sight::service::ut::locked_srv::INPUT);
        locked_srv->set_inout(inout, sight::service::ut::locked_srv::INOUT);
        locked_srv->set_output(output, sight::service::ut::locked_srv::OUTPUT);

        // Test that inputLock doesn't block other reader
        {
            auto weak_input = locked_srv->input<const sight::data::integer>(
                sight::service::ut::locked_srv::INPUT
            );
            auto shared_input = weak_input.lock();
            CHECK_EQ(input, shared_input.get_shared());

            std::thread t1(&sight::service::ut::locked_srv::starting, locked_srv);
            std::this_thread::sleep_for(std::chrono::milliseconds(500));

            // t1 should be in the sleep_for, so m_started and m_input should still be the initial value
            CHECK_EQ(std::int64_t(-1), locked_srv->m_input_value.load());
            CHECK_EQ(false, locked_srv->m_started.load());

            // Wait for t1 execution (1s)
            t1.join();

            CHECK_EQ(std::int64_t(0), locked_srv->m_input_value.load());
            CHECK_EQ(true, locked_srv->m_started.load());
        }

        // Test that outputLock is blocking
        {
            // Start thread immediately
            std::thread t2(&sight::service::ut::locked_srv::stopping, locked_srv);
            std::this_thread::sleep_for(std::chrono::milliseconds(500));

            // t2 should be in the sleep_for, so m_stopped should still be the initial value
            CHECK_EQ(false, locked_srv->m_stopped.load());

            {
                // We should be blocked here, as long as t2 is alive
                auto weak_output = locked_srv->output<sight::data::integer>(
                    sight::service::ut::locked_srv::OUTPUT
                );
                auto shared_output = weak_output.lock();

                // Once t2 have finished, we should be able to overwrite output
                shared_output->set_value(666);
            }

            t2.join();

            CHECK_EQ(true, locked_srv->m_stopped.load());
            CHECK_EQ(std::int64_t(666), output->get_value());
        }
    }
} // end TEST_SUITE

} // namespace sight::service::ut
