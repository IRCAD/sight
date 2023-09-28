/************************************************************************
 *
 * Copyright (C) 2009-2023 IRCAD France
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

#include "buffer_manager_test.hpp"

#include <core/memory/buffer_manager.hpp>
#include <core/memory/buffer_object.hpp>
#include <core/memory/policy/always_dump.hpp>
#include <core/memory/policy/barrier_dump.hpp>
#include <core/memory/policy/base.hpp>
#include <core/memory/policy/never_dump.hpp>
#include <core/memory/policy/valve_dump.hpp>

#include <utest/wait.hpp>

#include <cstddef>

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION(sight::core::memory::ut::buffer_manager_test);

namespace sight::core::memory::ut
{

//------------------------------------------------------------------------------

void buffer_manager_test::setUp()
{
    // Set up context before running a test.
}

//------------------------------------------------------------------------------

void buffer_manager_test::tearDown()
{
    // Clean up after the test run.
}

//------------------------------------------------------------------------------

void buffer_manager_test::allocate_test()
{
    core::memory::buffer_manager::sptr manager = core::memory::buffer_manager::get();

    const int SIZE                       = 100000;
    core::memory::buffer_object::sptr bo = std::make_shared<core::memory::buffer_object>();

    CPPUNIT_ASSERT(bo->is_empty());
    CPPUNIT_ASSERT(bo->lock().buffer() == nullptr);

    bo->allocate(SIZE);

    CPPUNIT_ASSERT(!bo->is_empty());
    CPPUNIT_ASSERT_EQUAL(static_cast<core::memory::buffer_object::size_t>(SIZE), bo->size());
    CPPUNIT_ASSERT(bo->lock().buffer() != nullptr);

    // We need to wait before checking that the buffer was unlocked because all buffer operations are done on a worker.
    // The actual buffer ref count might still be owned (as a std::promise) by the worker task when we reach this point.
    SIGHT_TEST_WAIT(bo->lock_count() == 0);
    CPPUNIT_ASSERT_EQUAL(static_cast<std::int64_t>(0), bo->lock_count());

    {
        core::memory::buffer_object::lock_t lock(bo->lock());

        SIGHT_TEST_WAIT(bo->lock_count() == 1);
        CPPUNIT_ASSERT_EQUAL(static_cast<std::int64_t>(1), bo->lock_count());
        char* buf = static_cast<char*>(lock.buffer());

        for(int i = 0 ; i < SIZE ; ++i)
        {
            buf[i] = static_cast<char>(i % 256);
        }
    }

    {
        core::memory::buffer_object::lock_t lock(bo->lock());
        char* buf = static_cast<char*>(lock.buffer());

        for(int i = 0 ; i < SIZE ; ++i)
        {
            CPPUNIT_ASSERT_EQUAL(static_cast<char>(i % 256), buf[i]);
        }
    }

    SIGHT_TEST_WAIT(bo->lock_count() == 0);
    CPPUNIT_ASSERT_EQUAL(static_cast<std::int64_t>(0), bo->lock_count());

    {
        core::memory::buffer_object::lock_t lock(bo->lock());

        SIGHT_TEST_WAIT(bo->lock_count() == 1);
        CPPUNIT_ASSERT_EQUAL(static_cast<std::int64_t>(1), bo->lock_count());
        core::memory::buffer_object::lock_t lock2(bo->lock());
        SIGHT_TEST_WAIT(bo->lock_count() == 2);
        CPPUNIT_ASSERT_EQUAL(static_cast<std::int64_t>(2), bo->lock_count());
        core::memory::buffer_object::csptr cbo = bo;
        core::memory::buffer_object::const_lock_t clock(cbo->lock());
        SIGHT_TEST_WAIT(bo->lock_count() == 3);
        CPPUNIT_ASSERT_EQUAL(static_cast<std::int64_t>(3), bo->lock_count());
    }

    SIGHT_TEST_WAIT(bo->lock_count() == 0);
    CPPUNIT_ASSERT_EQUAL(static_cast<std::int64_t>(0), bo->lock_count());

    bo->destroy();

    CPPUNIT_ASSERT(bo->is_empty());
    CPPUNIT_ASSERT(bo->lock().buffer() == nullptr);

    CPPUNIT_ASSERT(bo->is_empty());
    CPPUNIT_ASSERT(bo->lock().buffer() == nullptr);

    bo->allocate(SIZE, std::make_shared<core::memory::buffer_new_policy>());

    CPPUNIT_ASSERT(!bo->is_empty());
    CPPUNIT_ASSERT_EQUAL(static_cast<core::memory::buffer_object::size_t>(SIZE), bo->size());
    CPPUNIT_ASSERT(bo->lock().buffer() != nullptr);

    {
        core::memory::buffer_object::lock_t lock(bo->lock());
        char* buf = static_cast<char*>(lock.buffer());

        for(int i = 0 ; i < SIZE ; ++i)
        {
            buf[i] = static_cast<char>(i % 256);
        }
    }

    {
        core::memory::buffer_object::lock_t lock(bo->lock());
        char* buf = static_cast<char*>(lock.buffer());

        for(int i = 0 ; i < SIZE ; ++i)
        {
            CPPUNIT_ASSERT_EQUAL(static_cast<char>(i % 256), buf[i]);
        }
    }

    bo->destroy();

    CPPUNIT_ASSERT(bo->is_empty());
    CPPUNIT_ASSERT(bo->lock().buffer() == nullptr);
}

//------------------------------------------------------------------------------

void buffer_manager_test::memory_info_test()
{
    core::memory::buffer_manager::sptr manager = core::memory::buffer_manager::get();

    {
        SIGHT_INFO(manager->to_string().get());
        core::memory::buffer_object::sptr bo = std::make_shared<core::memory::buffer_object>();
        const int SIZE                       = 100000;
        bo->allocate(SIZE);
        SIGHT_INFO(manager->to_string().get());
        core::memory::buffer_object::sptr bo1 = std::make_shared<core::memory::buffer_object>();
        SIGHT_INFO(manager->to_string().get());
        {
            core::memory::buffer_object::lock_t lock1(bo1->lock());
            SIGHT_INFO(manager->to_string().get());
        }
        core::memory::buffer_object::sptr bo2 = std::make_shared<core::memory::buffer_object>();
        SIGHT_INFO(manager->to_string().get());
        bo->reallocate(SIZE * std::size_t(2));
        {
            core::memory::buffer_object::lock_t lock(bo->lock());
            SIGHT_INFO(manager->to_string().get());
        }
        bo->destroy();
        SIGHT_INFO(manager->to_string().get());
        bo1->allocate(SIZE);
        bo2->allocate(SIZE);
        char* buff = new char [SIZE];
        bo->set_buffer(buff, SIZE, std::make_shared<core::memory::buffer_new_policy>());
        SIGHT_INFO(manager->to_string().get());

        {
            core::memory::buffer_object::lock_t lock(bo->lock());
        }
        {
            core::memory::buffer_object::lock_t lock(bo1->lock());
        }
        {
            core::memory::buffer_object::lock_t lock(bo2->lock());
        }

        bo->destroy();
        bo1->destroy();
        bo2->destroy();
    }
    SIGHT_INFO(manager->to_string().get());
}

//------------------------------------------------------------------------------

void buffer_manager_test::swap_test()
{
    core::memory::buffer_manager::sptr manager      = core::memory::buffer_manager::get();
    core::memory::buffer_object::sptr bo1           = std::make_shared<core::memory::buffer_object>();
    core::memory::buffer_object::sptr bo2           = std::make_shared<core::memory::buffer_object>();
    core::memory::buffer_allocation_policy::sptr p1 = std::make_shared<core::memory::buffer_malloc_policy>();
    core::memory::buffer_allocation_policy::sptr p2 = std::make_shared<core::memory::buffer_new_policy>();
    bo1->allocate(sizeof(int), p1);
    bo2->allocate(sizeof(std::int64_t), p2);
    void* b1 = bo1->lock().buffer();
    void* b2 = bo2->lock().buffer();
    *static_cast<int*>(b1)          = 1;
    *static_cast<std::int64_t*>(b2) = 2L;
    bo1->swap(bo2);
    core::memory::buffer_manager::buffer_info_map_t map = manager->get_buffer_infos().get();
    b1 = bo1->lock().buffer();
    b2 = bo2->lock().buffer();
    CPPUNIT_ASSERT_EQUAL(static_cast<std::int64_t>(2), *static_cast<std::int64_t*>(b1));
    CPPUNIT_ASSERT_EQUAL(p2, map[bo1->get_buffer_pointer()].buffer_policy);
    CPPUNIT_ASSERT_EQUAL(sizeof(std::int64_t), map[bo1->get_buffer_pointer()].size);
    CPPUNIT_ASSERT_EQUAL(1, *static_cast<int*>(b2));
    CPPUNIT_ASSERT_EQUAL(p1, map[bo2->get_buffer_pointer()].buffer_policy);
    CPPUNIT_ASSERT_EQUAL(sizeof(int), map[bo2->get_buffer_pointer()].size);
    bo1->destroy();
    bo2->destroy();
}

//------------------------------------------------------------------------------

void buffer_manager_test::dump_restore_test()
{
    core::memory::buffer_manager::sptr manager       = core::memory::buffer_manager::get();
    core::memory::buffer_object::sptr bo             = std::make_shared<core::memory::buffer_object>();
    core::memory::buffer_manager::buffer_stats stats = manager->get_buffer_stats().get();
    const core::memory::buffer_manager::size_t zero  = 0;
    CPPUNIT_ASSERT_EQUAL(zero, stats.total_managed);
    CPPUNIT_ASSERT_EQUAL(zero, stats.total_dumped);

    bo->allocate(sizeof(char));
    stats = manager->get_buffer_stats().get();
    CPPUNIT_ASSERT_EQUAL(sizeof(char), stats.total_managed);
    CPPUNIT_ASSERT_EQUAL(zero, stats.total_dumped);
    *static_cast<char*>(bo->lock().buffer()) = '!';

    manager->dump_buffer(bo->get_buffer_pointer()).wait();
    stats = manager->get_buffer_stats().get();
    CPPUNIT_ASSERT_EQUAL(sizeof(char), stats.total_managed);
    CPPUNIT_ASSERT_EQUAL(sizeof(char), stats.total_dumped);

    core::memory::buffer_manager::buffer_info_map_t map = manager->get_buffer_infos().get();
    std::ifstream fs(map[bo->get_buffer_pointer()].fs_file.string());
    char x = 0;
    fs >> x;
    CPPUNIT_ASSERT_EQUAL('!', x);

    manager->restore_buffer(bo->get_buffer_pointer()).wait();
    stats = manager->get_buffer_stats().get();
    CPPUNIT_ASSERT_EQUAL(sizeof(char), stats.total_managed);
    CPPUNIT_ASSERT_EQUAL(zero, stats.total_dumped);
    CPPUNIT_ASSERT_EQUAL('!', *static_cast<char*>(bo->lock().buffer()));

    bo->destroy();
    stats = manager->get_buffer_stats().get();
    CPPUNIT_ASSERT_EQUAL(zero, stats.total_managed);
    CPPUNIT_ASSERT_EQUAL(zero, stats.total_dumped);
}

//------------------------------------------------------------------------------

class dummy_memory_monitor_tools
{
public:

    //------------------------------------------------------------------------------

    static std::uint64_t get_free_system_memory()
    {
        return s_free_system_memory;
    }

    //------------------------------------------------------------------------------

    static void set_free_system_memory(std::uint64_t new_memory)
    {
        s_free_system_memory = new_memory;
    }

private:

    static std::uint64_t s_free_system_memory;
};

std::uint64_t dummy_memory_monitor_tools::s_free_system_memory = 1024L * 1024;

//------------------------------------------------------------------------------

void buffer_manager_test::dump_policy_test()
{
    core::memory::buffer_manager::sptr manager = core::memory::buffer_manager::get();

    // Always Dump
    {
        auto always = std::make_shared<core::memory::policy::always_dump>();
        CPPUNIT_ASSERT(always->get_param_names().empty());
        CPPUNIT_ASSERT(!always->set_param("", ""));
        bool ok = false;
        CPPUNIT_ASSERT_EQUAL(std::string(""), always->get_param("", &ok));
        CPPUNIT_ASSERT(!ok);

        // The Always Dump policy isn't active yet: the buffer is still loaded
        core::memory::buffer_object::sptr bo = std::make_shared<core::memory::buffer_object>();
        bo->allocate(1);
        core::memory::buffer_info info = manager->get_buffer_infos().get().at(bo->get_buffer_pointer());
        CPPUNIT_ASSERT(info.loaded);

        // The Always Dump policy is active: the buffer is immediately dumped
        manager->set_dump_policy(always);
        info = manager->get_buffer_infos().get().at(bo->get_buffer_pointer());
        CPPUNIT_ASSERT(!info.loaded);

        // Lock request to modify the buffer: temporarily restore then redump the buffer
        *static_cast<char*>(bo->lock().buffer()) = '!';
        info                                     = manager->get_buffer_infos().get().at(bo->get_buffer_pointer());
        CPPUNIT_ASSERT(!info.loaded);

        // Reallocation: the buffer should be dumped after the reallocation
        bo->reallocate(2);
        info = manager->get_buffer_infos().get().at(bo->get_buffer_pointer());
        //CPPUNIT_ASSERT(!info.loaded); // TODO: Make it work

        bo->destroy();
    }

    // Barrier Dump
    {
        auto barrier =
            std::make_shared<core::memory::policy::barrier_dump>();
        const core::memory::policy::base::param_names_type& params = barrier->get_param_names();
        CPPUNIT_ASSERT(params.size() == 1 && params[0] == "barrier");
        CPPUNIT_ASSERT(!barrier->set_param("banner", "nope"));
        CPPUNIT_ASSERT(!barrier->set_param("barrier", "-1B"));
        CPPUNIT_ASSERT(barrier->set_param("barrier", "1B"));
        manager->set_dump_policy(barrier);

        // Allocation of a buffer: The managed memory size is still acceptable
        core::memory::buffer_object::sptr bo1 = std::make_shared<core::memory::buffer_object>();
        bo1->allocate(1);
        core::memory::buffer_manager::buffer_info_map_t infos = manager->get_buffer_infos().get();
        CPPUNIT_ASSERT(infos[bo1->get_buffer_pointer()].loaded);

        // Lock request to modify a buffer: Shouldn't change its loading status
        *static_cast<char*>(bo1->lock().buffer()) = '!';
        infos                                     = manager->get_buffer_infos().get();
        CPPUNIT_ASSERT(infos[bo1->get_buffer_pointer()].loaded);

        // Allocation of a second buffer: The managed memory size exceed the barrier, one of the
        // buffer is evicted from managed memory
        core::memory::buffer_object::sptr bo2 = std::make_shared<core::memory::buffer_object>();
        bo2->allocate(1);
        infos = manager->get_buffer_infos().get();
        CPPUNIT_ASSERT(infos[bo1->get_buffer_pointer()].loaded ^ infos[bo2->get_buffer_pointer()].loaded);

        // Reallocation of both buffer: No buffer fits the barrier, both buffers should be evicted
        // from managed memory (TODO: Make it work)
        /*bo1->reallocate(2);
           bo2->reallocate(2);
           infos = manager->get_buffer_infos().get();
           CPPUNIT_ASSERT(!infos[bo1->get_buffer_pointer()].loaded && !infos[bo2->get_buffer_pointer()].loaded);*/

        bo1->destroy();
        bo2->destroy();
    }

    // Never Dump
    {
        auto never = std::make_shared<core::memory::policy::never_dump>();
        CPPUNIT_ASSERT(never->get_param_names().empty());
        CPPUNIT_ASSERT(!never->set_param("", ""));
        bool ok = false;
        CPPUNIT_ASSERT_EQUAL(std::string(""), never->get_param("", &ok));
        CPPUNIT_ASSERT(!ok);

        // The buffer isn't dumped on allocation
        core::memory::buffer_object::sptr bo = std::make_shared<core::memory::buffer_object>();
        bo->allocate(1);
        core::memory::buffer_info info = manager->get_buffer_infos().get().at(bo->get_buffer_pointer());
        CPPUNIT_ASSERT(info.loaded);

        // The buffer isn't dumped when activating the new policy
        manager->set_dump_policy(never);
        info = manager->get_buffer_infos().get().at(bo->get_buffer_pointer());
        CPPUNIT_ASSERT(info.loaded);

        // Lock request to modify the buffer: the buffer still isn't dumped
        *static_cast<char*>(bo->lock().buffer()) = '!';
        info                                     = manager->get_buffer_infos().get().at(bo->get_buffer_pointer());
        CPPUNIT_ASSERT(info.loaded);

        // Reallocation: the buffer still isn't dumped
        bo->reallocate(2);
        info = manager->get_buffer_infos().get().at(bo->get_buffer_pointer());
        CPPUNIT_ASSERT(info.loaded);

        bo->destroy();
    }

    // Valve Dump
    {
        auto valve = std::make_shared<core::memory::policy::valve_dump<dummy_memory_monitor_tools> >();
        CPPUNIT_ASSERT_EQUAL(size_t(2), valve->get_param_names().size());
        CPPUNIT_ASSERT(!valve->set_param("min_free_memes", "nope"));
        CPPUNIT_ASSERT(!valve->set_param("hysteric_offset", "nope"));
        CPPUNIT_ASSERT(!valve->set_param("min_free_mem", "-1B"));
        CPPUNIT_ASSERT(!valve->set_param("hysteresis_offset", "-1B"));
        CPPUNIT_ASSERT(valve->set_param("min_free_mem", "2B"));
        CPPUNIT_ASSERT(valve->set_param("hysteresis_offset", "1B"));
        manager->set_dump_policy(valve);

        // Scenario 1: Eviction after allocation of a new buffer
        {
            dummy_memory_monitor_tools::set_free_system_memory(5);

            // Allocation of a buffer: the free memory is still acceptable
            core::memory::buffer_object::sptr bo1 = std::make_shared<core::memory::buffer_object>();
            bo1->allocate(1);
            dummy_memory_monitor_tools::set_free_system_memory(4);
            core::memory::buffer_manager::buffer_info_map_t infos = manager->get_buffer_infos().get();
            CPPUNIT_ASSERT(infos[bo1->get_buffer_pointer()].loaded);

            // Lock request to modify the buffer: shouldn't change its loading status as the system memory didn't change
            *static_cast<char*>(bo1->lock().buffer()) = '!';
            infos                                     = manager->get_buffer_infos().get();
            CPPUNIT_ASSERT(infos[bo1->get_buffer_pointer()].loaded);

            // Allocation of a second buffer: the free memory is still acceptable
            core::memory::buffer_object::sptr bo2 = std::make_shared<core::memory::buffer_object>();
            bo2->allocate(1);
            dummy_memory_monitor_tools::set_free_system_memory(3);
            infos = manager->get_buffer_infos().get();
            CPPUNIT_ASSERT(infos[bo1->get_buffer_pointer()].loaded && infos[bo2->get_buffer_pointer()].loaded);

            // Allocation of a third buffer: minimal free memory crossed, 2 buffers should be evicted (one for the
            // minimum memory and a second one for the hysteresis offset) (TODO: Make it work)
            /*core::memory::buffer_object::sptr bo3 = std::make_shared<core::memory::buffer_object>;
               bo3->allocate(1);
               Dummymemory_monitor_tools::setFreeSystemMemory(2);
               infos = manager->get_buffer_infos().get();
               CPPUNIT_ASSERT_EQUAL(
                1,
                infos[bo1->get_buffer_pointer()].loaded + infos[bo2->get_buffer_pointer()].loaded
             + infos[bo3->get_buffer_pointer()].loaded
               );*/

            bo1->destroy();
            bo2->destroy();
            //bo3->destroy();
        }

        // Scenario 2: Eviction after reallocation of an existing buffer
        {
            dummy_memory_monitor_tools::set_free_system_memory(5);

            // Allocation of a buffer: the free memory is still acceptable
            core::memory::buffer_object::sptr bo1 = std::make_shared<core::memory::buffer_object>();
            bo1->allocate(1);
            dummy_memory_monitor_tools::set_free_system_memory(4);
            core::memory::buffer_manager::buffer_info_map_t infos = manager->get_buffer_infos().get();
            CPPUNIT_ASSERT(infos[bo1->get_buffer_pointer()].loaded);

            // Allocation of a second buffer: the free memory is still acceptable
            core::memory::buffer_object::sptr bo2 = std::make_shared<core::memory::buffer_object>();
            bo2->allocate(1);
            dummy_memory_monitor_tools::set_free_system_memory(3);
            infos = manager->get_buffer_infos().get();
            CPPUNIT_ASSERT(infos[bo1->get_buffer_pointer()].loaded && infos[bo2->get_buffer_pointer()].loaded);

            // Reallocation of a buffer: minimal free memory crossed, at least one buffer should be evicted (at least
            // one for the minimum memory, possibly the second one for the hysteresis offset) (TODO: Make it work)
            /*bo1->reallocate(2);
               Dummymemory_monitor_tools::setFreeSystemMemory(2);
               infos = manager->get_buffer_infos().get();
               CPPUNIT_ASSERT(infos[bo1->get_buffer_pointer()].loaded + infos[bo2->get_buffer_pointer()].loaded <= 1);*/

            bo1->destroy();
            bo2->destroy();
        }

        // Scenario 3: Eviction because minimal free memory crossed for external reasons
        {
            dummy_memory_monitor_tools::set_free_system_memory(1024L * 1024);

            // Allocation of a buffer: the free memory is still acceptable
            core::memory::buffer_object::sptr bo = std::make_shared<core::memory::buffer_object>();
            bo->allocate(1);
            core::memory::buffer_info info = manager->get_buffer_infos().get().at(bo->get_buffer_pointer());
            CPPUNIT_ASSERT(info.loaded);

            // There is no more memory in your system because you launched <insert any application based on Electron>
            // The buffer isn't evicted yet, because the policy doesn't listen actively to the memory changes
            dummy_memory_monitor_tools::set_free_system_memory(0);
            info = manager->get_buffer_infos().get().at(bo->get_buffer_pointer());
            CPPUNIT_ASSERT(info.loaded);

            // When operating on the buffer, the policy will notice the lack of memory and it will be evicted
            bo->lock();
            info = manager->get_buffer_infos().get().at(bo->get_buffer_pointer());
            CPPUNIT_ASSERT(!info.loaded);

            bo->destroy();
        }
    }
}

} // namespace sight::core::memory::ut
