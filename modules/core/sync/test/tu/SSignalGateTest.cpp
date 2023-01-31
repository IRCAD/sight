/************************************************************************
 *
 * Copyright (C) 2022 IRCAD France
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

#include "SSignalGateTest.hpp"

#include <core/com/Signal.hxx>
#include <core/com/Slot.hxx>
#include <core/service/base.hpp>

#include <utest/wait.hpp>

CPPUNIT_TEST_SUITE_REGISTRATION(sight::module::sync::ut::SSignalGateTest);

namespace sight::module::sync::ut
{

class Object : public data::Object
{
public:

    Object()
    {
        newSignal<core::com::Signal<void()> >("signal");
    }

    //------------------------------------------------------------------------------

    void emitSignal()
    {
        signal<core::com::Signal<void()> >("signal")->emit();
    }
};

//------------------------------------------------------------------------------

void SSignalGateTest::setUp()
{
    m_signalGate = service::add("sight::module::sync::SSignalGate");
}

//------------------------------------------------------------------------------

void SSignalGateTest::tearDown()
{
    m_worker->stop();
    if(!m_signalGate->isStopped())
    {
        CPPUNIT_ASSERT_NO_THROW(m_signalGate->stop().get());
    }

    service::remove(m_signalGate);
}

//------------------------------------------------------------------------------

void SSignalGateTest::basicTest()
{
    auto object1 = std::make_shared<Object>();
    object1->setID("object1");
    auto object2 = std::make_shared<Object>();
    object2->setID("object2");
    auto object3 = std::make_shared<Object>();
    object3->setID("object3");
    bool allReceived     = false;
    auto allReceivedSlot = core::com::newSlot([&allReceived]{allReceived = true;});
    m_worker = core::thread::Worker::New();
    allReceivedSlot->setWorker(m_worker);
    m_signalGate->signal("allReceived")->connect(allReceivedSlot);
    boost::property_tree::ptree ptree;
    ptree.add("signal", "object1/signal");
    ptree.add("signal", "object2/signal");
    ptree.add("signal", "object3/signal");
    m_signalGate->setConfiguration(ptree);
    CPPUNIT_ASSERT_NO_THROW(m_signalGate->configure());
    CPPUNIT_ASSERT_NO_THROW(m_signalGate->start().get());

    object1->emitSignal();
    object2->emitSignal();
    object3->emitSignal();
    fwTestWaitMacro(allReceived);
    CPPUNIT_ASSERT(allReceived);
}

} // namespace sight::module::sync::ut
