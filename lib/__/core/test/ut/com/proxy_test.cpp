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

#include <core/com/proxy.hpp>
#include <core/com/signal.hpp>

#include <core/com/slot.hpp>

#include <utest/wait.hpp>

#include <doctest/doctest.h>

namespace
{

struct proxy_test_class
{
    proxy_test_class()
    = default;

    //------------------------------------------------------------------------------

    int sum(int _a, int _b)
    {
        SIGHT_INFO("SUM " << _a << " + " << _b);
        ++m_method_sum;
        return _a + _b;
    }

    //------------------------------------------------------------------------------

    int square(int _a)
    {
        SIGHT_INFO("SQUARE " << _a);
        ++m_method_square;
        return _a * _a;
    }

    //------------------------------------------------------------------------------

    void do_nothing()
    {
        SIGHT_INFO("DO NOTHING");
        ++m_method_do_nothing;
    }

    int m_method_sum {0};
    int m_method_square {0};
    int m_method_do_nothing {0};
};

} // namespace

TEST_SUITE("sight::core::com::proxy")
{
//------------------------------------------------------------------------------

    TEST_CASE("basic")
    {
        const std::string channel1 = "channel1";
        const std::string channel2 = "channel2";

        sight::core::com::proxy::sptr proxy = sight::core::com::proxy::get();

        const auto sig  = std::make_shared<sight::core::com::signal<void(int, int)> >();
        const auto sig2 = std::make_shared<sight::core::com::signal<void(int, int, char)> >();
        const auto sig3 = std::make_shared<sight::core::com::signal<void()> >();

        proxy_test_class test_object;
        const auto slot  = sight::core::com::new_slot(&proxy_test_class::sum, &test_object);
        const auto slot2 = sight::core::com::new_slot(&proxy_test_class::square, &test_object);
        const auto slot3 = sight::core::com::new_slot(
            &proxy_test_class::do_nothing,
            &test_object
        );
        const auto slot4                         = sight::core::com::new_slot([](const std::string ){return;});
        sight::core::thread::worker::sptr worker = sight::core::thread::worker::make();
        slot->set_worker(worker);
        slot2->set_worker(worker);
        slot3->set_worker(worker);
        slot4->set_worker(worker);

        proxy->connect(channel1, sig);
        proxy->connect(channel1, sig2);
        proxy->connect(channel1, slot);
        proxy->connect(channel1, slot2);
        proxy->connect(channel1, slot3);

        // This fails to connect, more parameters in the signal (int, int) AND not the same type (std::string)
        CHECK_NOTHROW(proxy->connect(channel1, slot4));

        CHECK_NOTHROW(proxy->connect(channel2, sig3));
        // This fails to connect, no parameter in the signal () while there is one ine the slot (std::string)
        CHECK_NOTHROW(proxy->connect(channel2, slot4));

        CHECK_EQ(static_cast<std::size_t>(3), sig->num_connections());
        CHECK_EQ(static_cast<std::size_t>(3), sig2->num_connections());
        CHECK_EQ(static_cast<std::size_t>(2), slot->num_connections());
        CHECK_EQ(static_cast<std::size_t>(2), slot2->num_connections());
        CHECK_EQ(static_cast<std::size_t>(2), slot3->num_connections());
        sig->async_emit(3, 5);

        SIGHT_TEST_WAIT(
            test_object.m_method_sum == 1 && test_object.m_method_square == 1 && test_object.m_method_do_nothing == 1
        );

        CHECK_EQ(1, test_object.m_method_sum);
        CHECK_EQ(1, test_object.m_method_square);
        CHECK_EQ(1, test_object.m_method_do_nothing);

        sig2->async_emit(8, 2, 'x');

        SIGHT_TEST_WAIT(
            test_object.m_method_sum == 2 && test_object.m_method_square == 2 && test_object.m_method_do_nothing == 2
        );

        CHECK_EQ(2, test_object.m_method_sum);
        CHECK_EQ(2, test_object.m_method_square);
        CHECK_EQ(2, test_object.m_method_do_nothing);

        proxy->disconnect(channel1, sig);
        proxy->disconnect(channel1, sig2);
        proxy->disconnect(channel1, slot);
        proxy->disconnect(channel1, slot2);
        proxy->disconnect(channel1, slot3);

        proxy->disconnect(channel2, sig3);

        CHECK_EQ(static_cast<std::size_t>(0), sig->num_connections());
        CHECK_EQ(static_cast<std::size_t>(0), sig2->num_connections());
        CHECK_EQ(static_cast<std::size_t>(0), slot->num_connections());
        CHECK_EQ(static_cast<std::size_t>(0), slot2->num_connections());
        CHECK_EQ(static_cast<std::size_t>(0), slot3->num_connections());
        worker->stop();
    }
} // TEST_SUITE
