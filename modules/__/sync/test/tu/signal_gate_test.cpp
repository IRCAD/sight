/************************************************************************
 *
 * Copyright (C) 2022-2023 IRCAD France
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

#include "signal_gate_test.hpp"

#include <core/com/signal.hxx>
#include <core/com/slot.hxx>

#include <service/op.hpp>

#include <utest/wait.hpp>

CPPUNIT_TEST_SUITE_REGISTRATION(sight::module::sync::ut::signal_gate_test);

namespace sight::module::sync::ut
{

class object : public data::object
{
public:

    object()
    {
        new_signal<core::com::signal<void()> >("signal");
    }

    //------------------------------------------------------------------------------

    void emitSignal()
    {
        signal<core::com::signal<void()> >("signal")->emit();
    }
};

//------------------------------------------------------------------------------

void signal_gate_test::setUp()
{
    m_signalGate = service::add("sight::module::sync::signal_gate");
}

//------------------------------------------------------------------------------

void signal_gate_test::tearDown()
{
    m_worker->stop();
    if(!m_signalGate->stopped())
    {
        CPPUNIT_ASSERT_NO_THROW(m_signalGate->stop().get());
    }

    service::remove(m_signalGate);
}

//------------------------------------------------------------------------------

void signal_gate_test::basicTest()
{
    auto object1 = std::make_shared<object>();
    object1->set_id("object1");
    auto object2 = std::make_shared<object>();
    object2->set_id("object2");
    auto object3 = std::make_shared<object>();
    object3->set_id("object3");
    bool allReceived     = false;
    auto allReceivedSlot = core::com::new_slot([&allReceived]{allReceived = true;});
    m_worker = core::thread::worker::make();
    allReceivedSlot->set_worker(m_worker);
    m_signalGate->signal("allReceived")->connect(allReceivedSlot);
    boost::property_tree::ptree ptree;
    ptree.add("signal", "object1/signal");
    ptree.add("signal", "object2/signal");
    ptree.add("signal", "object3/signal");
    m_signalGate->set_config(ptree);
    CPPUNIT_ASSERT_NO_THROW(m_signalGate->configure());
    CPPUNIT_ASSERT_NO_THROW(m_signalGate->start().get());

    object1->emitSignal();
    object2->emitSignal();
    object3->emitSignal();
    SIGHT_TEST_WAIT(allReceived);
    CPPUNIT_ASSERT(allReceived);
}

} // namespace sight::module::sync::ut
