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

#include "slots_signals_stuff.hpp"

#include <service/registry.hpp>

#include <utest/exception.hpp>

#include <doctest/doctest.h>

#include <thread>

namespace sight::service::ut
{

static sight::utest::exception fw_test_exception(""); // force link with fwTest

TEST_SUITE("sight::service::slots_signals")
{
//------------------------------------------------------------------------------

    TEST_CASE("basic")
    {
        auto buffer1 = std::make_shared<sight::service::ut::buffer>();
        auto buffer2 = std::make_shared<sight::service::ut::buffer>();

        sight::core::thread::worker::sptr worker = sight::core::thread::worker::make();
        sight::core::thread::add_worker("test", worker);

        auto basic_test_srv = sight::service::factory::make<sight::service::ut::basic_test>();
        sight::service::register_service(basic_test_srv);
        basic_test_srv->set_inout(buffer1, sight::service::ut::basic_test::BUFFER_INOUT);

        basic_test_srv->set_worker(worker);

        auto start_future = basic_test_srv->start();
        CHECK(basic_test_srv->status() != sight::service::base::global_status::started);
        start_future.get();
        CHECK(basic_test_srv->status() == sight::service::base::global_status::started);

        auto update_future = basic_test_srv->update();
        CHECK_EQ(basic_test_srv->m_update_finished, false);
        update_future.get();
        CHECK_EQ(basic_test_srv->m_update_finished, true);

        auto swap_future = basic_test_srv->swap_key(sight::service::ut::basic_test::BUFFER_INOUT, buffer2);
        CHECK_EQ(basic_test_srv->m_swap_finished, false);
        swap_future.get();
        CHECK_EQ(basic_test_srv->m_swap_finished, true);

        auto stop_future = basic_test_srv->stop();
        CHECK(basic_test_srv->status() != sight::service::base::global_status::stopped);
        stop_future.get();
        CHECK(basic_test_srv->status() == sight::service::base::global_status::stopped);

        sight::service::unregister_service(basic_test_srv);

        sight::core::thread::remove_worker("test");
    }

//------------------------------------------------------------------------------

    TEST_CASE("com_object_service")
    {
        sight::core::thread::worker::sptr worker1 = sight::core::thread::worker::make();
        sight::core::thread::add_worker("worker1", worker1);
        sight::core::thread::worker::sptr worker2 = sight::core::thread::worker::make();
        sight::core::thread::add_worker("worker2", worker2);

        auto buffer1 = std::make_shared<sight::service::ut::buffer>();
        {
            auto reader_test_srv = sight::service::factory::make<sight::service::ut::reader_test>();
            sight::service::register_service(reader_test_srv);
            reader_test_srv->set_inout(buffer1, sight::service::ut::basic_srv::BUFFER_INOUT);

            auto show_test_srv = sight::service::factory::make<sight::service::ut::show_test>();
            sight::service::register_service(show_test_srv);
            show_test_srv->set_inout(buffer1, sight::service::ut::basic_srv::BUFFER_INOUT);
            show_test_srv->set_worker(worker1);

            buffer1->signal(sight::data::signals::MODIFIED)->connect(
                show_test_srv->slot(
                    sight::service::slots::UPDATE
                )
            );

            reader_test_srv->start();
            show_test_srv->start();

            reader_test_srv->update().get();

            sight::service::base::shared_future_t stop_reader_future = reader_test_srv->stop();
            sight::service::base::shared_future_t stop_show_future   = show_test_srv->stop();
            stop_reader_future.get();
            stop_show_future.get();

            CHECK_EQ(show_test_srv->m_receive_count, 1);

            buffer1->signal(sight::data::signals::MODIFIED)->disconnect(
                show_test_srv->slot(
                    sight::service::slots::UPDATE
                )
            );

            sight::service::unregister_service(reader_test_srv);
            sight::service::unregister_service(show_test_srv);
        }

        {
            auto reader_test_srv = sight::service::factory::make<sight::service::ut::reader_test>();
            sight::service::register_service(reader_test_srv);
            reader_test_srv->set_inout(buffer1, sight::service::ut::basic_srv::BUFFER_INOUT);

            auto reader2_test_srv = sight::service::factory::make<sight::service::ut::reader_test>();
            sight::service::register_service(reader2_test_srv);
            reader2_test_srv->set_inout(buffer1, sight::service::ut::basic_srv::BUFFER_INOUT);
            reader2_test_srv->set_worker(worker2);

            auto show_test_srv = sight::service::factory::make<sight::service::ut::show_test>();
            sight::service::register_service(show_test_srv);
            show_test_srv->set_inout(buffer1, sight::service::ut::basic_srv::BUFFER_INOUT, true);
            show_test_srv->set_worker(worker1);

            reader_test_srv->start();
            reader2_test_srv->start();
            show_test_srv->start().get();

            sight::service::base::shared_future_t update_reader_future  = reader_test_srv->update();
            sight::service::base::shared_future_t update_reader2_future = reader2_test_srv->update();
            update_reader_future.get();
            update_reader2_future.get();

            sight::service::base::shared_future_t stop_reader_future  = reader_test_srv->stop();
            sight::service::base::shared_future_t stop_reader2_future = reader2_test_srv->stop();
            sight::service::base::shared_future_t stop_show_future    = show_test_srv->stop();
            stop_reader_future.get();
            stop_reader2_future.get();
            stop_show_future.get();

            CHECK_EQ(show_test_srv->m_receive_count, 2);

            sight::service::unregister_service(reader_test_srv);
            sight::service::unregister_service(reader2_test_srv);
            sight::service::unregister_service(show_test_srv);
        }

        sight::core::thread::remove_worker("worker1");
        sight::core::thread::remove_worker("worker2");
    }

//------------------------------------------------------------------------------

    TEST_CASE("com_service_to_service")
    {
        auto buffer1 = std::make_shared<sight::service::ut::buffer>();

        sight::core::thread::worker::sptr worker1 = sight::core::thread::worker::make();
        sight::core::thread::add_worker("worker1", worker1);

        auto reader_test_srv = sight::service::factory::make<sight::service::ut::reader2_test>();
        sight::service::register_service(reader_test_srv);
        reader_test_srv->set_inout(buffer1, sight::service::ut::basic_srv::BUFFER_INOUT);

        auto show_test_srv = sight::service::factory::make<sight::service::ut::show_test>();
        sight::service::register_service(show_test_srv);
        show_test_srv->set_inout(buffer1, sight::service::ut::basic_srv::BUFFER_INOUT);
        show_test_srv->set_worker(worker1);

        reader_test_srv->signal(sight::service::ut::reader2_test::signals::CHANGED)->connect(
            show_test_srv->slot(
                sight::service::ut::show_test::slots::CHANGE
            )
        );

        reader_test_srv->start();
        show_test_srv->start();

        reader_test_srv->update().get();

        sight::service::base::shared_future_t stop_reader_future = reader_test_srv->stop();
        sight::service::base::shared_future_t stop_show_future   = show_test_srv->stop();
        stop_reader_future.get();
        stop_show_future.get();

        reader_test_srv->signal(sight::service::ut::reader2_test::signals::CHANGED)->disconnect(
            show_test_srv->slot(
                sight::service::ut::show_test::slots::CHANGE
            )
        );

        CHECK_EQ(show_test_srv->m_change_count, 1);

        sight::service::unregister_service(reader_test_srv);
        sight::service::unregister_service(show_test_srv);

        sight::core::thread::remove_worker("worker1");
    }

//------------------------------------------------------------------------------

    TEST_CASE("block_connection")
    {
        auto buffer1 = std::make_shared<sight::service::ut::buffer>();

        sight::core::thread::worker::sptr worker1 = sight::core::thread::worker::make();
        sight::core::thread::add_worker("worker1", worker1);

        auto reader_test_srv = sight::service::factory::make<sight::service::ut::reader_test>();
        sight::service::register_service(reader_test_srv);
        reader_test_srv->set_inout(buffer1, sight::service::ut::basic_srv::BUFFER_INOUT);

        auto show_test_srv = sight::service::factory::make<sight::service::ut::show2_test>();
        sight::service::register_service(show_test_srv);
        show_test_srv->set_inout(buffer1, sight::service::ut::basic_srv::BUFFER_INOUT);
        show_test_srv->set_worker(worker1);

        sight::core::com::connection connection;
        connection = buffer1->signal(sight::data::signals::MODIFIED)->
                     connect(show_test_srv->slot(sight::service::ut::show2_test::slots::UPDATE_BUFFER));

        reader_test_srv->start();
        show_test_srv->start();

        reader_test_srv->update();

        std::this_thread::sleep_for(std::chrono::seconds(8));

        sight::service::base::shared_future_t stop_reader_future = reader_test_srv->stop();
        sight::service::base::shared_future_t stop_show_future   = show_test_srv->stop();
        stop_reader_future.get();
        stop_show_future.get();

        connection.disconnect();

        CHECK_EQ(show_test_srv->m_receive_count, 1);

        sight::core::thread::remove_worker("worker1");
    }
} // end TEST_SUITE

} // namespace sight::service::ut
