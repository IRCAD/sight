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

#include <doctest/doctest.h>

TEST_SUITE("sight::service::sig_slot_connection")
{
    TEST_CASE("basic")
    {
        auto buffer_data = std::make_shared<sight::service::ut::buffer>();

        auto show_test_srv = sight::service::factory::make<sight::service::ut::show_test>();
        sight::service::register_service(show_test_srv);
        show_test_srv->set_inout(buffer_data, sight::service::ut::basic_srv::BUFFER_INOUT, true);
        show_test_srv->set_worker(sight::core::thread::get_default_worker());

        buffer_data->async_emit(sight::data::signals::MODIFIED);
        CHECK_EQ(0, show_test_srv->m_receive_count);

        show_test_srv->start().get();
        buffer_data->async_emit(sight::data::signals::MODIFIED);
        show_test_srv->stop().get();
        CHECK_EQ(1, show_test_srv->m_receive_count);

        buffer_data->async_emit(sight::data::signals::MODIFIED);
        CHECK_EQ(1, show_test_srv->m_receive_count);

        sight::service::unregister_service(show_test_srv);
    }
}
