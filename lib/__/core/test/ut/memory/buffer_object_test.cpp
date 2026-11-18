/************************************************************************
 *
 * Copyright (C) 2009-2026 IRCAD France
 * Copyright (C) 2012-2021 IHU Strasbourg
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

#include <core/memory/buffer_allocation_policy.hpp>
#include <core/memory/buffer_object.hpp>
#include <core/memory/exception/memory.hpp>

#include <utest/wait.hpp>

#include <doctest/doctest.h>

#include <array>
#include <cstdint>
#include <limits>
#include <thread>
#include <type_traits>

TEST_SUITE("sight::core::memory::buffer_object")
{
//------------------------------------------------------------------------------

    template<typename T>
    static bool is_pointed_value_const(T /*unused*/)
    {
        return std::is_const_v<std::remove_pointer_t<T> >;
    }

//------------------------------------------------------------------------------

    TEST_CASE("allocate")
    {
        const std::size_t size                      = 100000;
        sight::core::memory::buffer_object::sptr bo = std::make_shared<sight::core::memory::buffer_object>();

        CHECK(bo->is_empty());
        CHECK(bo->lock().buffer() == nullptr);

        bo->allocate(size);

        CHECK(!bo->is_empty());
        CHECK_EQ(static_cast<sight::core::memory::buffer_object::size_t>(size), bo->size());
        CHECK(bo->lock().buffer() != nullptr);

        // We need to wait before checking that the buffer was unlocked because all buffer operations are done on a
        // worker.
        // The actual buffer ref count might still be owned (as a std::promise) by the worker task when we reach this
        // point.
        SIGHT_TEST_WAIT(bo->lock_count() == 0);
        CHECK_EQ(static_cast<std::int64_t>(0), bo->lock_count());

        {
            sight::core::memory::buffer_object::lock_t lock(bo->lock());
            SIGHT_TEST_WAIT(bo->lock_count() == 1);
            CHECK_EQ(static_cast<std::int64_t>(1), bo->lock_count());
            char* buf = static_cast<char*>(lock.buffer());

            for(std::size_t i = 0 ; i < size ; ++i)
            {
                buf[i] = static_cast<char>(i % 256);
            }
        }

        {
            sight::core::memory::buffer_object::lock_t lock(bo->lock());
            char* buf = static_cast<char*>(lock.buffer());

            for(std::size_t i = 0 ; i < size ; ++i)
            {
                CHECK_EQ(static_cast<char>(i % 256), buf[i]);
            }
        }

        SIGHT_TEST_WAIT(bo->lock_count() == 0);
        CHECK_EQ(static_cast<std::int64_t>(0), bo->lock_count());

        {
            sight::core::memory::buffer_object::lock_t lock(bo->lock());

            SIGHT_TEST_WAIT(bo->lock_count() == 1);
            CHECK_EQ(static_cast<std::int64_t>(1), bo->lock_count());
            sight::core::memory::buffer_object::lock_t lock2(bo->lock());
            SIGHT_TEST_WAIT(bo->lock_count() == 2);
            CHECK_EQ(static_cast<std::int64_t>(2), bo->lock_count());
            sight::core::memory::buffer_object::csptr cbo = bo;
            sight::core::memory::buffer_object::const_lock_t clock(cbo->lock());
            SIGHT_TEST_WAIT(bo->lock_count() == 3);
            CHECK_EQ(static_cast<std::int64_t>(3), bo->lock_count());
            CHECK(is_pointed_value_const(clock.buffer()));
            CHECK(is_pointed_value_const(cbo->lock().buffer()));
        }

        SIGHT_TEST_WAIT(bo->lock_count() == 0);
        CHECK_EQ(static_cast<std::int64_t>(0), bo->lock_count());

        bo->destroy();

        CHECK(bo->is_empty());
        CHECK(bo->lock().buffer() == nullptr);

        CHECK(bo->is_empty());
        CHECK(bo->lock().buffer() == nullptr);

        bo->allocate(size, std::make_shared<sight::core::memory::buffer_new_policy<> >());

        CHECK(!bo->is_empty());
        CHECK_EQ(static_cast<sight::core::memory::buffer_object::size_t>(size), bo->size());
        CHECK(bo->lock().buffer() != nullptr);

        {
            sight::core::memory::buffer_object::lock_t lock(bo->lock());
            char* buf = static_cast<char*>(lock.buffer());

            for(std::size_t i = 0 ; i < size ; ++i)
            {
                buf[i] = static_cast<char>(i % 256);
            }
        }

        {
            sight::core::memory::buffer_object::lock_t lock(bo->lock());
            char* buf = static_cast<char*>(lock.buffer());

            for(std::size_t i = 0 ; i < size ; ++i)
            {
                CHECK_EQ(static_cast<char>(i % 256), buf[i]);
            }
        }

        bo->destroy();
        CHECK(bo->is_empty());
        CHECK(bo->lock().buffer() == nullptr);

        CHECK_THROWS_AS(
            bo->allocate(std::numeric_limits<std::size_t>::max() / 2),
            sight::core::memory::exception::memory
        );

        bo->allocate(size);
        CHECK(!bo->is_empty());
        CHECK_EQ(static_cast<sight::core::memory::buffer_object::size_t>(size), bo->size());
        CHECK(bo->lock().buffer() != nullptr);

        CHECK_THROWS_AS(
            bo->reallocate(std::numeric_limits<std::size_t>::max() / 2),
            sight::core::memory::exception::memory
        );

        CHECK(!bo->is_empty());
        CHECK_EQ(static_cast<sight::core::memory::buffer_object::size_t>(size), bo->size());
        CHECK(bo->lock().buffer() != nullptr);

        const std::size_t smaller_realloc_size = 1024;
        const std::size_t bigger_realloc_size  = size + 1024;

        bo->reallocate(smaller_realloc_size);
        CHECK(!bo->is_empty());
        CHECK_EQ(static_cast<sight::core::memory::buffer_object::size_t>(smaller_realloc_size), bo->size());
        CHECK(bo->lock().buffer() != nullptr);

        bo->reallocate(bigger_realloc_size);
        CHECK(!bo->is_empty());
        CHECK_EQ(static_cast<sight::core::memory::buffer_object::size_t>(bigger_realloc_size), bo->size());
        CHECK(bo->lock().buffer() != nullptr);

        bo->destroy();

        CHECK(bo->is_empty());
        CHECK(bo->lock().buffer() == nullptr);

        CHECK_THROWS_AS(
            bo->allocate(150, std::make_shared<sight::core::memory::buffer_no_alloc_policy>()),
            sight::core::memory::exception::memory
        );
        // CHECK_THROWS_AS( bo->reallocate(150), sight::core::memory::exception::memory);
    }

//------------------------------------------------------------------------------

    TEST_CASE("reallocate_external_buffers_with_a_shared_new_policy")
    {
        auto policy = std::make_shared<sight::core::memory::buffer_new_policy<std::int32_t> >();
        auto first  = std::make_shared<sight::core::memory::buffer_object>();
        auto second = std::make_shared<sight::core::memory::buffer_object>();

        first->set_buffer(new std::int32_t[4] {1, 2, 3, 4}, 4 * sizeof(std::int32_t), policy, true);
        second->set_buffer(new std::int32_t[2] {5, 6}, 2 * sizeof(std::int32_t), policy, true);

        first->reallocate(8 * sizeof(std::int32_t));
        {
            const auto lock    = first->lock();
            const auto* values = static_cast<const std::int32_t*>(lock.buffer());
            for(std::size_t i = 0 ; i < 4 ; ++i)
            {
                CHECK_EQ(values[i], static_cast<std::int32_t>(i + 1));
            }
        }

        second->reallocate(sizeof(std::int32_t));
        {
            const auto lock = second->lock();
            CHECK_EQ(*static_cast<const std::int32_t*>(lock.buffer()), 5);
        }

        first->reallocate(2 * sizeof(std::int32_t));
        {
            const auto lock    = first->lock();
            const auto* values = static_cast<const std::int32_t*>(lock.buffer());
            CHECK_EQ(values[0], 1);
            CHECK_EQ(values[1], 2);
        }
    }

//------------------------------------------------------------------------------

    TEST_CASE("allocate_zero")
    {
        const std::size_t size                      = 100000;
        sight::core::memory::buffer_object::sptr bo = std::make_shared<sight::core::memory::buffer_object>();

        CHECK(bo->is_empty());
        CHECK(bo->lock().buffer() == nullptr);

        bo->allocate(0);

        CHECK(bo->is_empty());
        CHECK(bo->lock().buffer() == nullptr);

        bo->allocate(size);

        CHECK(!bo->is_empty());
        CHECK_EQ(static_cast<sight::core::memory::buffer_object::size_t>(size), bo->size());
        CHECK(bo->lock().buffer() != nullptr);

        bo->destroy();
    }

//------------------------------------------------------------------------------

    static void stress_lock(sight::core::memory::buffer_object::sptr _bo, int _nb_locks, int _nb_test)
    {
        std::vector<sight::core::memory::buffer_object::lock_t> m_locks;

        for(int t = 0 ; t < _nb_test ; ++t)
        {
            for(int i = 0 ; i < _nb_locks ; ++i)
            {
                m_locks.push_back(_bo->lock());
            }

            SIGHT_TEST_WAIT(_bo->lock_count() >= 3);
            CHECK(_bo->lock_count() >= static_cast<std::int64_t>(3));

            m_locks.clear();
        }
    }

//------------------------------------------------------------------------------

    TEST_CASE("lock_threaded_stress")
    {
        sight::core::memory::buffer_object::sptr bo = std::make_shared<sight::core::memory::buffer_object>();

        {
            // Joined at the end of the scope.
            std::array<std::jthread, 4> threads {
                std::jthread([bo]{stress_lock(bo, 800, 600);}),
                std::jthread([bo]{stress_lock(bo, 600, 800);}),
                std::jthread([bo]{stress_lock(bo, 452, 692);}),
                std::jthread([bo]{stress_lock(bo, 253, 345);})
            };
        }

        SIGHT_TEST_WAIT(bo->lock_count() == 0);
        CHECK_EQ(static_cast<std::int64_t>(0), bo->lock_count());
    }
} // TEST_SUITE
