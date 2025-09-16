/************************************************************************
 *
 * Copyright (C) 2009-2025 IRCAD France
 * Copyright (C) 2012-2015 IHU Strasbourg
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

#include <core/lazy_instantiator.hpp>
#include <core/mt/types.hpp>

#include <doctest/doctest.h>

#include <chrono>
#include <thread>
#include <vector>

TEST_SUITE("sight::core::lazy_instantiator")
{
//-----------------------------------------------------------------------------

    template<int SLEEP = 0>
    class static_counter
    {
    public:

        using sptr = std::shared_ptr<static_counter>;

        static_counter()
        {
            sight::core::mt::scoped_lock lock(s_mutex);
            ++s_counter;
            std::this_thread::sleep_for(std::chrono::seconds(SLEEP));
        }

        static int s_counter;
        static sight::core::mt::mutex s_mutex;
    };

    template<int SLEEP>
    int static_counter<SLEEP>::s_counter = 0;

    template<int SLEEP>
    sight::core::mt::mutex static_counter<SLEEP>::s_mutex;

    struct second_counter {};

    TEST_CASE("lazy")
    {
        static_counter<>::s_counter = 0;

        static_counter<> cpt;

        CHECK_EQ(1, cpt.s_counter);

        static_counter<>::sptr counter1 = sight::core::lazy_instantiator<static_counter<> >::get_instance();
        CHECK_EQ(2, counter1->s_counter);

        static_counter<>::sptr counter2 = sight::core::lazy_instantiator<static_counter<> >::get_instance();
        CHECK_EQ(2, counter2->s_counter);

        static_counter<>::sptr counter3 = sight::core::lazy_instantiator<static_counter<>,
                                                                         second_counter>::get_instance();
        CHECK_EQ(3, counter3->s_counter);

        static_counter<>::s_counter = 0;
    }

    struct thread_counter_tag {};

    struct counter_thread
    {
        using counter_type = static_counter<5>;
        using sptr         = std::shared_ptr<counter_thread>;

        counter_thread() = default;

        //------------------------------------------------------------------------------

        static void run()
        {
            counter_type::sptr counter;
            counter = sight::core::lazy_instantiator<counter_type, thread_counter_tag>::get_instance();
        }
    };

//-----------------------------------------------------------------------------

    TEST_CASE("thread_safety")
    {
        const int nb_thread(100);
        counter_thread::counter_type::s_counter = 0;

        std::vector<std::thread> tg;
        for(std::size_t i = 0 ; i <= nb_thread ; i++)
        {
            counter_thread::sptr ct = std::make_shared<counter_thread>();
            tg.emplace_back(&counter_thread::run);
        }

        for(auto& t : tg)
        {
            t.join();
        }

        counter_thread::counter_type::sptr counter;
        counter = sight::core::lazy_instantiator<counter_thread::counter_type, thread_counter_tag>::get_instance();
        CHECK_EQ(1, counter->s_counter);

        counter_thread::counter_type::s_counter = 0;
    }
} // TEST_SUITE
