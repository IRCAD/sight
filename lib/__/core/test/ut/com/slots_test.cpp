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

#include <core/com/has_slots.hpp>
#include <core/com/slots.hpp>
#include <core/com/slots.hxx>
#include <core/thread/worker.hpp>

#include <doctest/doctest.h>

namespace
{

struct slots_test_basic
{
    //------------------------------------------------------------------------------

    static int sum(int _a, int _b)
    {
        return _a + _b;
    }
};

} // namespace

TEST_SUITE("sight::core::com::slots")
{
//------------------------------------------------------------------------------

    static int slots_test_sum(int _a, int _b)
    {
        return _a + _b;
    }

//------------------------------------------------------------------------------

    static void slots_test_print(const std::string& _str)
    {
        SIGHT_DEBUG(_str);
    }

//-----------------------------------------------------------------------------

    TEST_CASE("build")
    {
        sight::core::com::slots slots;

        auto slot1 = sight::core::com::new_slot(&slots_test_sum);
        auto slot2 = sight::core::com::new_slot(&slots_test_print);

        slots("sum", std::dynamic_pointer_cast<sight::core::com::slot_base>(slot1))
            ("print", std::dynamic_pointer_cast<sight::core::com::slot_base>(slot2))
            ("another_key", slot1);

        CHECK(slot1 == slots["sum"]);
        CHECK(slot2 == slots["print"]);
        CHECK(slots["sum"] == slots["another_key"]);
        CHECK(!slots["wrong_slot"]);

        slots("struct_sum", slot1);

        CHECK_EQ(slots["struct_sum"]->call<int>(5, 9), 14);

        auto worker = sight::core::thread::worker::make();

        slots.set_worker(worker);

        int count = 0;
        for(const sight::core::com::slots::key_t& key : slots.get_slot_keys())
        {
            sight::core::com::slot_base::sptr slot = slots[key];
            CHECK(worker == slot->get_worker());
            ++count;
        }

        CHECK_EQ(count, 4);
        worker->stop();
    }

    namespace
    {

//-----------------------------------------------------------------------------

    struct slots_test_has_slot : public sight::core::com::has_slots
    {
        using get_value_slot_type = sight::core::com::slot<int ()>;

        slots_test_has_slot()
        {
            new_slot("getValue", &slots_test_has_slot::get_value);
            new_slot("sum", &slots_test_has_slot::sum);
        }

        //------------------------------------------------------------------------------

        static int sum(int _a, int _b)
        {
            return _a + _b;
        }

        //------------------------------------------------------------------------------

        static int get_value()
        {
            return 4;
        }
    };

//-----------------------------------------------------------------------------

    struct slots_test_has_slot2 : public sight::core::com::has_slots
    {
        using get_value_slot_type = sight::core::com::slot<int ()>;

        slots_test_has_slot2()
        {
            new_slot("sum", &slots_test_has_slot2::sum);

            auto slot = new_slot("getValue", &slots_test_has_slot2::get_value);
            CHECK(slot);
        }

        //------------------------------------------------------------------------------

        static int sum(int _a, int _b)
        {
            return _a + _b;
        }

        //------------------------------------------------------------------------------

        static int get_value()
        {
            return 4;
        }
    };

    } // namespace

//-----------------------------------------------------------------------------

    TEST_CASE("has_slots")
    {
        slots_test_has_slot obj;
        CHECK_EQ(obj.slot("sum")->call<int>(5, 9), 14);
        CHECK_EQ(obj.slot<slots_test_has_slot::get_value_slot_type>("getValue")->call(), 4);

        slots_test_has_slot2 obj2;
        CHECK_EQ(obj2.slot("sum")->call<int>(5, 9), 14);
        CHECK_EQ(obj2.slot<slots_test_has_slot::get_value_slot_type>("getValue")->call(), 4);
    }

//-----------------------------------------------------------------------------
} // TEST_SUITE
