/************************************************************************
 *
 * Copyright (C) 2022-2026 IRCAD France
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

#include <core/com/signal.hpp>
#include <core/com/signal.hxx>
#include <core/com/slot.hxx>

#include <service/op.hpp>

#include <utest/wait.hpp>

#include <doctest/doctest.h>

class object : public sight::data::object
{
public:

    object()
    {
        new_signal<sight::core::com::signal<void()> >("signal");
    }

    //------------------------------------------------------------------------------

    void emit_signal()
    {
        signal<sight::core::com::signal<void()> >("signal")->emit();
    }
};

//------------------------------------------------------------------------------

TEST_SUITE("sight::module::sync::signal_gate")
{
    TEST_CASE("basic")
    {
        auto m_signal_gate = sight::service::add("sight::module::sync::signal_gate");

        auto object1 = std::make_shared<object>();
        object1->set_id("object1");
        auto object2 = std::make_shared<object>();
        object2->set_id("object2");
        auto object3 = std::make_shared<object>();
        object3->set_id("object3");
        bool all_received      = false;
        auto all_received_slot = sight::core::com::new_slot([&all_received]{all_received = true;});
        auto m_worker          = sight::core::thread::worker::make();
        all_received_slot->set_worker(m_worker);
        m_signal_gate->signal("all_received")->connect(all_received_slot);
        boost::property_tree::ptree ptree;
        ptree.add("signal", "object1/signal");
        ptree.add("signal", "object2/signal");
        ptree.add("signal", "object3/signal");
        m_signal_gate->set_config(ptree);
        CHECK_NOTHROW(m_signal_gate->configure());
        CHECK_NOTHROW(m_signal_gate->start().get());

        object1->emit_signal();
        object2->emit_signal();
        object3->emit_signal();
        SIGHT_TEST_WAIT(all_received);
        CHECK(all_received);

        // Cleanup
        m_worker->stop();
        if(!m_signal_gate->stopped())
        {
            CHECK_NOTHROW(m_signal_gate->stop().get());
        }

        sight::service::remove(m_signal_gate);
    }
}
