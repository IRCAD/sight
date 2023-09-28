/************************************************************************
 *
 * Copyright (C) 2009-2023 IRCAD France
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

#include "slots_test.hpp"

#include <core/com/has_slots.hpp>
#include <core/com/slots.hpp>
#include <core/com/slots.hxx>
#include <core/thread/worker.hpp>

#include <utest/exception.hpp>

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION(sight::core::com::ut::slots_test);

namespace sight::core::com::ut
{

//------------------------------------------------------------------------------

void slots_test::setUp()
{
    // Set up context before running a test.
}

//------------------------------------------------------------------------------

void slots_test::tearDown()
{
    // Clean up after the test run.
}

//------------------------------------------------------------------------------

int slots_test_sum(int a, int b)
{
    return a + b;
}

//------------------------------------------------------------------------------

void slots_test_print(const std::string& str)
{
    SIGHT_DEBUG(str);
}

struct slots_test_basic
{
    slots_test_basic()
    = default;

    //------------------------------------------------------------------------------

    static int sum(int a, int b)
    {
        return a + b;
    }
};

//-----------------------------------------------------------------------------

void slots_test::build_test()
{
    core::com::slots slots;

    core::com::slot<int(int, int)>::sptr slot1            = core::com::new_slot(&slots_test_sum);
    core::com::slot<void(const std::string&)>::sptr slot2 = core::com::new_slot(&slots_test_print);

    slots("sum", std::dynamic_pointer_cast<slot_base>(slot1))
        ("print", std::dynamic_pointer_cast<slot_base>(slot2))
        ("another_key", slot1);

    CPPUNIT_ASSERT(slot1 == slots["sum"]);
    CPPUNIT_ASSERT(slot2 == slots["print"]);
    CPPUNIT_ASSERT(slots["sum"] == slots["another_key"]);
    CPPUNIT_ASSERT(!slots["wrong_slot"]);

    slots("struct_sum", slot1);

    CPPUNIT_ASSERT_EQUAL(14, slots["struct_sum"]->call<int>(5, 9));

    core::thread::worker::sptr worker = core::thread::worker::make();

    slots.set_worker(worker);

    int count = 0;
    for(const core::com::slots::key_t& key : slots.get_slot_keys())
    {
        core::com::slot_base::sptr slot = slots[key];
        CPPUNIT_ASSERT(worker == slot->get_worker());
        ++count;
    }

    CPPUNIT_ASSERT_EQUAL(4, count);
    worker->stop();
}

//-----------------------------------------------------------------------------

struct slots_test_has_slot : public has_slots
{
    using get_value_slot_type = core::com::slot<int ()>;

    slots_test_has_slot()
    {
        auto slot_get_value = core::com::new_slot(&slots_test_has_slot::get_value);
        auto slot_sum       = core::com::new_slot(&slots_test_has_slot::sum);

        has_slots::m_slots("sum", slot_sum)
            ("getValue", slot_get_value);
    }

    //------------------------------------------------------------------------------

    static int sum(int a, int b)
    {
        return a + b;
    }

    //------------------------------------------------------------------------------

    static int get_value()
    {
        return 4;
    }
};

//-----------------------------------------------------------------------------

struct slots_test_has_slot2 : public has_slots
{
    using get_value_slot_type = core::com::slot<int ()>;

    slots_test_has_slot2()
    {
        new_slot("sum", &slots_test_has_slot2::sum);

        get_value_slot_type::sptr slot = new_slot("getValue", &slots_test_has_slot2::get_value);
        CPPUNIT_ASSERT(slot);
    }

    //------------------------------------------------------------------------------

    static int sum(int a, int b)
    {
        return a + b;
    }

    //------------------------------------------------------------------------------

    static int get_value()
    {
        return 4;
    }
};

//-----------------------------------------------------------------------------

void slots_test::has_slots_test()
{
    slots_test_has_slot obj;
    CPPUNIT_ASSERT_EQUAL(14, obj.slot("sum")->call<int>(5, 9));
    CPPUNIT_ASSERT_EQUAL(4, obj.slot<slots_test_has_slot::get_value_slot_type>("getValue")->call());

    slots_test_has_slot2 obj2;
    CPPUNIT_ASSERT_EQUAL(14, obj2.slot("sum")->call<int>(5, 9));
    CPPUNIT_ASSERT_EQUAL(4, obj2.slot<slots_test_has_slot::get_value_slot_type>("getValue")->call());
}

//-----------------------------------------------------------------------------

void slots_test::slots_idtest()
{
}

//-----------------------------------------------------------------------------

} // namespace sight::core::com::ut
