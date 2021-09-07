/************************************************************************
 *
 * Copyright (C) 2009-2021 IRCAD France
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

#include "SignalTest.hpp"

#include "core/com/exception/AlreadyConnected.hpp"
#include "core/com/exception/BadSlot.hpp"

#include <core/com/Signal.hpp>
#include <core/com/Signal.hxx>
#include <core/com/Slot.hpp>
#include <core/com/Slot.hxx>
#include <core/thread/Worker.hpp>

#include <boost/date_time/posix_time/posix_time.hpp>

#include <string>

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION(sight::core::com::ut::SignalTest);

namespace sight::core::com
{

namespace ut
{

//------------------------------------------------------------------------------

void SignalTest::setUp()
{
    // Set up context before running a test.
}

//------------------------------------------------------------------------------

void SignalTest::tearDown()
{
    // Clean up after the test run.
}

//-----------------------------------------------------------------------------

void SignalTest::buildTest()
{
    {
        typedef void Signature();
        core::com::Signal<Signature>::sptr sig = core::com::Signal<Signature>::New();
        CPPUNIT_ASSERT(sig);
    }

    {
        typedef void Signature(int);
        core::com::Signal<Signature>::sptr sig = core::com::Signal<Signature>::New();
        CPPUNIT_ASSERT(sig);
    }

    {
        typedef void Signature(char, float);
        core::com::Signal<Signature>::sptr sig = core::com::Signal<Signature>::New();
        CPPUNIT_ASSERT(sig);
    }

    {
        typedef void Signature(short, double, std::string);
        core::com::Signal<Signature>::sptr sig = core::com::Signal<Signature>::New();
        CPPUNIT_ASSERT(sig);
    }
}

//-----------------------------------------------------------------------------

struct SignalTestClass
{
    SignalTestClass() :
        m_method00(false),
        m_method0(false),
        m_method1(false),
        m_method2(false),
        m_method3(false)
    {
    }

    //------------------------------------------------------------------------------

    void method0()
    {
        m_method0 = true;
    }

    //------------------------------------------------------------------------------

    void method00()
    {
        m_method00 = true;
    }

    //------------------------------------------------------------------------------

    float method1(float f)
    {
        m_method1 = true;
        return 2 * f;
    }

    //------------------------------------------------------------------------------

    float method2(float f, int)
    {
        m_method2 = true;
        return 2 * f;
    }

    //------------------------------------------------------------------------------

    float method3(float f, double, std::string)
    {
        m_method3 = true;
        return 2 * f;
    }

    bool m_method00;
    bool m_method0;
    bool m_method1;
    bool m_method2;
    bool m_method3;
};

//-----------------------------------------------------------------------------

void SignalTest::connectTest()
{
    SignalTestClass testObject;
    core::com::Slot<void()>::sptr slot0 =
        core::com::newSlot(&SignalTestClass::method0, &testObject);
    core::com::Slot<float(float)>::sptr slot1 =
        core::com::newSlot(&SignalTestClass::method1, &testObject);
    core::com::Slot<float(float, int)>::sptr slot2 =
        core::com::newSlot(&SignalTestClass::method2, &testObject);
    core::com::Slot<float(float, double, std::string)>::sptr slot3 =
        core::com::newSlot(&SignalTestClass::method3, &testObject);

    core::com::Connection connection;
    CPPUNIT_ASSERT(connection.expired());

    {
        typedef void Signature();
        core::com::Signal<Signature>::sptr sig = core::com::Signal<Signature>::New();

        connection = sig->connect(slot0);
        CPPUNIT_ASSERT(!connection.expired());
        CPPUNIT_ASSERT_EQUAL((size_t) 1, sig->getNumberOfConnections());
    }

    CPPUNIT_ASSERT(connection.expired());

    {
        typedef void Signature(float);
        core::com::Signal<Signature>::sptr sig = core::com::Signal<Signature>::New();

        connection = sig->connect(slot1);
        CPPUNIT_ASSERT(!connection.expired());
        CPPUNIT_ASSERT_EQUAL((size_t) 1, sig->getNumberOfConnections());
    }

    CPPUNIT_ASSERT(connection.expired());

    {
        typedef void Signature(float, int);
        core::com::Signal<Signature>::sptr sig = core::com::Signal<Signature>::New();

        connection = sig->connect(slot2);
        CPPUNIT_ASSERT(!connection.expired());
        CPPUNIT_ASSERT_EQUAL((size_t) 1, sig->getNumberOfConnections());
    }

    CPPUNIT_ASSERT(connection.expired());

    {
        typedef void Signature(float, double, std::string);
        core::com::Signal<Signature>::sptr sig = core::com::Signal<Signature>::New();

        connection = sig->connect(slot3);
        CPPUNIT_ASSERT(!connection.expired());
        CPPUNIT_ASSERT_EQUAL((size_t) 1, sig->getNumberOfConnections());
    }

    CPPUNIT_ASSERT(connection.expired());

    // Disconnect
    {
        typedef void Signature();
        core::com::Signal<Signature>::sptr sig = core::com::Signal<Signature>::New();

        connection = sig->connect(slot0);
        CPPUNIT_ASSERT(!connection.expired());
        CPPUNIT_ASSERT_EQUAL((size_t) 1, sig->getNumberOfConnections());
        connection.disconnect();
        CPPUNIT_ASSERT(connection.expired());
        CPPUNIT_ASSERT_EQUAL((size_t) 0, sig->getNumberOfConnections());
    }

    {
        typedef void Signature();
        core::com::Signal<Signature>::sptr sig = core::com::Signal<Signature>::New();

        sig->connect(slot0);
        connection = sig->getConnection(slot0);

        CPPUNIT_ASSERT(!connection.expired());
        CPPUNIT_ASSERT_EQUAL((size_t) 1, sig->getNumberOfConnections());
        connection.disconnect();
        CPPUNIT_ASSERT(connection.expired());
        CPPUNIT_ASSERT_EQUAL((size_t) 0, sig->getNumberOfConnections());

        CPPUNIT_ASSERT_THROW(sig->getConnection(slot3, true), core::com::exception::BadSlot);
    }

    {
        typedef void Signature();
        core::com::Signal<Signature>::sptr sig = core::com::Signal<Signature>::New();

        connection = sig->connect(slot0);
        CPPUNIT_ASSERT(!connection.expired());
        CPPUNIT_ASSERT_EQUAL((size_t) 1, sig->getNumberOfConnections());
        sig->disconnect(slot0);
        CPPUNIT_ASSERT(connection.expired());
        CPPUNIT_ASSERT_EQUAL((size_t) 0, sig->getNumberOfConnections());
    }

    {
        typedef void Signature();
        core::com::Signal<Signature>::sptr sig = core::com::Signal<Signature>::New();

        core::com::Slot<void()>::sptr slot =
            core::com::newSlot(&SignalTestClass::method0, &testObject);

        connection = sig->connect(slot0);
        core::com::Connection connection2 = sig->connect(slot);

        CPPUNIT_ASSERT(!connection.expired());
        CPPUNIT_ASSERT(!connection2.expired());
        CPPUNIT_ASSERT_EQUAL((size_t) 2, sig->getNumberOfConnections());
        sig->disconnectAll();
        CPPUNIT_ASSERT(connection.expired());
        CPPUNIT_ASSERT(connection2.expired());
        CPPUNIT_ASSERT_EQUAL((size_t) 0, sig->getNumberOfConnections());
    }

    {
        typedef void Signature(std::string);
        core::com::Signal<Signature>::sptr sig = core::com::Signal<Signature>::New();

        CPPUNIT_ASSERT_THROW(sig->connect(slot1), core::com::exception::BadSlot);
        CPPUNIT_ASSERT_THROW(sig->connect(slot2), core::com::exception::BadSlot);
        CPPUNIT_ASSERT_THROW(sig->connect(slot3), core::com::exception::BadSlot);
    }

    {
        typedef void Signature(std::string);
        core::com::Signal<Signature>::sptr sig = core::com::Signal<Signature>::New();

        CPPUNIT_ASSERT_THROW(sig->disconnect(slot1), core::com::exception::BadSlot);
        CPPUNIT_ASSERT_THROW(sig->disconnect(slot2), core::com::exception::BadSlot);
        CPPUNIT_ASSERT_THROW(sig->disconnect(slot3), core::com::exception::BadSlot);

        sig->connect(slot0);

        CPPUNIT_ASSERT_THROW(sig->disconnect(slot1), core::com::exception::BadSlot);
        CPPUNIT_ASSERT_THROW(sig->disconnect(slot2), core::com::exception::BadSlot);
        CPPUNIT_ASSERT_THROW(sig->disconnect(slot3), core::com::exception::BadSlot);

        sig->disconnect(slot0);
    }

    {
        typedef void Signature();
        core::com::Signal<Signature>::sptr sig = core::com::Signal<Signature>::New();

        sig->connect(slot0);
        CPPUNIT_ASSERT_THROW(sig->connect(slot0), core::com::exception::AlreadyConnected);

        CPPUNIT_ASSERT_EQUAL((size_t) 1, sig->getNumberOfConnections());
    }
}

//-----------------------------------------------------------------------------

void SignalTest::emitTest()
{
    core::com::Connection connection;

    {
        typedef void Signature();
        SignalTestClass testObject;

        core::com::Slot<Signature>::sptr slot =
            core::com::newSlot(&SignalTestClass::method0, &testObject);

        core::com::Signal<Signature>::sptr sig = core::com::Signal<Signature>::New();

        connection = sig->connect(slot);
        CPPUNIT_ASSERT(!connection.expired());

        CPPUNIT_ASSERT_EQUAL((size_t) 1, sig->getNumberOfConnections());
        sig->emit();
        CPPUNIT_ASSERT(testObject.m_method0);
    }

    CPPUNIT_ASSERT(connection.expired());

    {
        typedef void Signature(float);
        SignalTestClass testObject;

        core::com::Slot<float(float)>::sptr slot =
            core::com::newSlot(&SignalTestClass::method1, &testObject);

        core::com::Signal<Signature>::sptr sig = core::com::Signal<Signature>::New();

        connection = sig->connect(slot);
        CPPUNIT_ASSERT(!connection.expired());

        CPPUNIT_ASSERT_EQUAL((size_t) 1, sig->getNumberOfConnections());

        sig->emit(21.0f);
        CPPUNIT_ASSERT(testObject.m_method1);
    }

    CPPUNIT_ASSERT(connection.expired());

    {
        typedef void Signature(float, int);
        SignalTestClass testObject;

        core::com::Slot<float(float, int)>::sptr slot =
            core::com::newSlot(&SignalTestClass::method2, &testObject);

        core::com::Signal<Signature>::sptr sig = core::com::Signal<Signature>::New();

        connection = sig->connect(slot);
        CPPUNIT_ASSERT(!connection.expired());

        CPPUNIT_ASSERT_EQUAL((size_t) 1, sig->getNumberOfConnections());

        sig->emit(21.0f, 42);
        CPPUNIT_ASSERT(testObject.m_method2);
    }

    CPPUNIT_ASSERT(connection.expired());

    {
        typedef void Signature(float, double, std::string);
        SignalTestClass testObject;

        core::com::Slot<float(float, double, std::string)>::sptr slot =
            core::com::newSlot(&SignalTestClass::method3, &testObject);

        core::com::Signal<Signature>::sptr sig = core::com::Signal<Signature>::New();

        connection = sig->connect(slot);
        CPPUNIT_ASSERT(!connection.expired());

        CPPUNIT_ASSERT_EQUAL((size_t) 1, sig->getNumberOfConnections());

        sig->emit(21.0f, 42.0, "emit");
        CPPUNIT_ASSERT(testObject.m_method3);
    }

    CPPUNIT_ASSERT(connection.expired());
}

//-----------------------------------------------------------------------------

void SignalTest::autoSlotDisconnectTest()
{
    typedef void Signature(float);
    SignalTestClass testObject;

    core::com::Signal<Signature>::sptr sig = core::com::Signal<Signature>::New();

    {
        core::com::Slot<void()>::sptr slot0 =
            core::com::newSlot(&SignalTestClass::method0, &testObject);

        sig->connect(slot0);

        CPPUNIT_ASSERT_EQUAL((size_t) 1, sig->getNumberOfConnections());
        {
            core::com::Slot<float(float)>::sptr slot1 =
                core::com::newSlot(&SignalTestClass::method1, &testObject);

            sig->connect(slot1);
            CPPUNIT_ASSERT_EQUAL((size_t) 2, sig->getNumberOfConnections());
        }

        CPPUNIT_ASSERT_EQUAL((size_t) 1, sig->getNumberOfConnections());

        {
            core::com::Slot<void()>::sptr slot00 =
                core::com::newSlot(&SignalTestClass::method00, &testObject);

            sig->connect(slot00);
            CPPUNIT_ASSERT_EQUAL((size_t) 2, sig->getNumberOfConnections());
        }

        CPPUNIT_ASSERT_EQUAL((size_t) 1, sig->getNumberOfConnections());
    }

    CPPUNIT_ASSERT_EQUAL((size_t) 0, sig->getNumberOfConnections());
}

//-----------------------------------------------------------------------------

void SignalTest::argumentLossTest()
{
    typedef void Signature(float, double, std::string);
    SignalTestClass testObject;

    core::com::Signal<Signature>::sptr sig = core::com::Signal<Signature>::New();

    core::com::Slot<void()>::sptr slot0 =
        core::com::newSlot(&SignalTestClass::method0, &testObject);
    core::com::Slot<float(float)>::sptr slot1 =
        core::com::newSlot(&SignalTestClass::method1, &testObject);
    core::com::Slot<float(float, double, std::string)>::sptr slot3 =
        core::com::newSlot(&SignalTestClass::method3, &testObject);

    sig->connect(slot0);
    CPPUNIT_ASSERT_EQUAL((size_t) 1, sig->getNumberOfConnections());

    sig->connect(slot1);
    CPPUNIT_ASSERT_EQUAL((size_t) 2, sig->getNumberOfConnections());

    sig->connect(slot3);
    CPPUNIT_ASSERT_EQUAL((size_t) 3, sig->getNumberOfConnections());

    sig->emit(21.0f, 42.0, "emit");

    CPPUNIT_ASSERT(testObject.m_method0);
    CPPUNIT_ASSERT(testObject.m_method1);
    CPPUNIT_ASSERT(testObject.m_method3);

    sig->disconnectAll();
    CPPUNIT_ASSERT_EQUAL((size_t) 0, sig->getNumberOfConnections());
}

//-----------------------------------------------------------------------------

void SignalTest::asyncArgumentLossTest()
{
    typedef void Signature(float, double, std::string);
    SignalTestClass testObject;

    core::com::Signal<Signature>::sptr sig = core::com::Signal<Signature>::New();

    core::com::Slot<void()>::sptr slot0 =
        core::com::newSlot(&SignalTestClass::method0, &testObject);
    core::com::Slot<float(float)>::sptr slot1 =
        core::com::newSlot(&SignalTestClass::method1, &testObject);
    core::com::Slot<float(float, double, std::string)>::sptr slot3 =
        core::com::newSlot(&SignalTestClass::method3, &testObject);

    core::thread::Worker::sptr worker = core::thread::Worker::New();
    slot0->setWorker(worker);
    slot1->setWorker(worker);
    slot3->setWorker(worker);

    sig->connect(slot0);
    CPPUNIT_ASSERT_EQUAL((size_t) 1, sig->getNumberOfConnections());

    sig->connect(slot1);
    CPPUNIT_ASSERT_EQUAL((size_t) 2, sig->getNumberOfConnections());

    sig->connect(slot3);
    CPPUNIT_ASSERT_EQUAL((size_t) 3, sig->getNumberOfConnections());

    sig->asyncEmit(21.0f, 42.0, "asyncEmit");

    std::this_thread::sleep_for(std::chrono::milliseconds(100));

    CPPUNIT_ASSERT(testObject.m_method0);
    CPPUNIT_ASSERT(testObject.m_method1);
    CPPUNIT_ASSERT(testObject.m_method3);

    sig->disconnectAll();
    CPPUNIT_ASSERT_EQUAL((size_t) 0, sig->getNumberOfConnections());
    worker->stop();
}

//-----------------------------------------------------------------------------

void SignalTest::blockTest()
{
    typedef void Signature(float, double, std::string);
    SignalTestClass testObject;

    core::com::Connection connection;

    core::com::Signal<Signature>::sptr sig = core::com::Signal<Signature>::New();

    core::com::Slot<void()>::sptr slot0 =
        core::com::newSlot(&SignalTestClass::method0, &testObject);
    core::com::Slot<float(float)>::sptr slot1 =
        core::com::newSlot(&SignalTestClass::method1, &testObject);
    core::com::Slot<float(float, double, std::string)>::sptr slot3 =
        core::com::newSlot(&SignalTestClass::method3, &testObject);

    sig->connect(slot0);
    connection = sig->connect(slot1);
    sig->connect(slot3);

    CPPUNIT_ASSERT_EQUAL((size_t) 3, sig->getNumberOfConnections());

    sig->emit(21.0f, 42.0, "emit");

    CPPUNIT_ASSERT(testObject.m_method0);
    CPPUNIT_ASSERT(testObject.m_method1);
    CPPUNIT_ASSERT(testObject.m_method3);

    testObject.m_method0 = false;
    testObject.m_method1 = false;
    testObject.m_method3 = false;

    {
        core::com::Connection::Blocker block(connection);
        sig->emit(21.0f, 42.0, "emit");
    }

    CPPUNIT_ASSERT(testObject.m_method0);
    CPPUNIT_ASSERT(!testObject.m_method1);
    CPPUNIT_ASSERT(testObject.m_method3);

    testObject.m_method0 = false;
    testObject.m_method1 = false;
    testObject.m_method3 = false;

    sig->emit(21.0f, 42.0, "emit");

    CPPUNIT_ASSERT(testObject.m_method0);
    CPPUNIT_ASSERT(testObject.m_method1);
    CPPUNIT_ASSERT(testObject.m_method3);

    testObject.m_method0 = false;
    testObject.m_method1 = false;
    testObject.m_method3 = false;

    {
        core::com::Connection::Blocker block(connection);
        block.reset();
        sig->emit(21.0f, 42.0, "emit");
    }

    CPPUNIT_ASSERT(testObject.m_method0);
    CPPUNIT_ASSERT(testObject.m_method1);
    CPPUNIT_ASSERT(testObject.m_method3);
}

//-----------------------------------------------------------------------------

void SignalTest::asyncEmitTest()
{
    core::com::Connection connection;
    core::thread::Worker::sptr worker = core::thread::Worker::New();

    {
        typedef void Signature();
        SignalTestClass testObject;

        core::com::Slot<Signature>::sptr slot =
            core::com::newSlot(&SignalTestClass::method0, &testObject);
        slot->setWorker(worker);

        core::com::Signal<Signature>::sptr sig = core::com::Signal<Signature>::New();

        connection = sig->connect(slot);
        CPPUNIT_ASSERT(!connection.expired());

        CPPUNIT_ASSERT_EQUAL((size_t) 1, sig->getNumberOfConnections());
        sig->asyncEmit();
        std::this_thread::sleep_for(std::chrono::seconds(1));

        CPPUNIT_ASSERT(testObject.m_method0);
    }

    CPPUNIT_ASSERT(connection.expired());

    {
        typedef void Signature(float);
        SignalTestClass testObject;

        core::com::Slot<float(float)>::sptr slot =
            core::com::newSlot(&SignalTestClass::method1, &testObject);
        slot->setWorker(worker);

        core::com::Signal<Signature>::sptr sig = core::com::Signal<Signature>::New();

        connection = sig->connect(slot);
        CPPUNIT_ASSERT(!connection.expired());

        CPPUNIT_ASSERT_EQUAL((size_t) 1, sig->getNumberOfConnections());

        sig->asyncEmit(21.0f);
        std::this_thread::sleep_for(std::chrono::seconds(1));

        CPPUNIT_ASSERT(testObject.m_method1);
    }

    CPPUNIT_ASSERT(connection.expired());

    {
        typedef void Signature(float, int);
        SignalTestClass testObject;

        core::com::Slot<float(float, int)>::sptr slot =
            core::com::newSlot(&SignalTestClass::method2, &testObject);
        slot->setWorker(worker);

        core::com::Signal<Signature>::sptr sig = core::com::Signal<Signature>::New();

        connection = sig->connect(slot);
        CPPUNIT_ASSERT(!connection.expired());

        CPPUNIT_ASSERT_EQUAL((size_t) 1, sig->getNumberOfConnections());

        sig->asyncEmit(21.0f, 42);
        std::this_thread::sleep_for(std::chrono::seconds(1));

        CPPUNIT_ASSERT(testObject.m_method2);
    }

    CPPUNIT_ASSERT(connection.expired());

    {
        typedef void Signature(float, double, std::string);
        SignalTestClass testObject;

        core::com::Slot<float(float, double, std::string)>::sptr slot =
            core::com::newSlot(&SignalTestClass::method3, &testObject);
        slot->setWorker(worker);

        core::com::Signal<Signature>::sptr sig = core::com::Signal<Signature>::New();

        connection = sig->connect(slot);
        CPPUNIT_ASSERT(!connection.expired());

        CPPUNIT_ASSERT_EQUAL((size_t) 1, sig->getNumberOfConnections());

        sig->asyncEmit(21.0f, 42.0, "emit");
        std::this_thread::sleep_for(std::chrono::seconds(1));

        CPPUNIT_ASSERT(testObject.m_method3);
    }

    CPPUNIT_ASSERT(connection.expired());
    worker->stop();
}

} //namespace ut

} //namespace sight::core::com
