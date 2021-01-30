/************************************************************************
 *
 * Copyright (C) 2009-2021 IRCAD France
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

#include "com/SignalsTest.hpp"

#include <core/com/HasSignals.hpp>
#include <core/com/Signal.hpp>
#include <core/com/Signal.hxx>
#include <core/com/Signals.hpp>

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION( sight::core::com::ut::SignalsTest );

namespace sight::core::com
{
namespace ut
{

//------------------------------------------------------------------------------

void SignalsTest::setUp()
{
    // Set up context before running a test.

}
//------------------------------------------------------------------------------

void SignalsTest::tearDown()
{
    // Clean up after the test run.
}

//-----------------------------------------------------------------------------

void SignalsTest::buildTest()
{
    core::com::Signals signals;

    core::com::Signal< void() >::sptr sig = core::com::Signal< void() >::New();
    CPPUNIT_ASSERT( sig );

    typedef void Signature (int, float);
    core::com::Signal< Signature >::sptr sig2 = core::com::Signal< Signature >::New();
    CPPUNIT_ASSERT( sig2 );

    signals("sig", sig)
        ("sig2", sig2);

    CPPUNIT_ASSERT(sig == signals["sig"]);
    CPPUNIT_ASSERT(sig2 == signals["sig2"]);
    CPPUNIT_ASSERT(!signals["wrong_signal"]);

    CPPUNIT_ASSERT_EQUAL((size_t)2, signals.getSignalKeys().size());
}

//-----------------------------------------------------------------------------

struct SignalsTestHasSignals : public HasSignals
{
    typedef core::com::Signal< void ()> SignalType;

    SignalsTestHasSignals()
    {
        SignalType::sptr signal = SignalType::New();
        HasSignals::m_signals("sig", signal);
    }
};

struct SignalsTestHasSignals2 : public HasSignals
{
    typedef core::com::Signal< void ()> SignalType;

    SignalsTestHasSignals2()
    {
        SignalType::sptr sig = this->newSignal< SignalType >("sig");
        CPPUNIT_ASSERT(sig);
    }
};

struct SignalsTestA
{

    SignalsTestA()
    {
        m_val = false;
    }

    //------------------------------------------------------------------------------

    void changeStatus()
    {
        m_val = !m_val;
    }

    bool m_val;
};
//-----------------------------------------------------------------------------

void SignalsTest::hasSignalsTest()
{
    {
        SignalsTestHasSignals obj;
        SignalsTestA srv;
        Slot< void()>::sptr slot = core::com::newSlot( &SignalsTestA::changeStatus, &srv );
        obj.signal("sig")->connect( slot );
        obj.signal< SignalsTestHasSignals::SignalType >("sig")->emit();
        CPPUNIT_ASSERT( srv.m_val );
        obj.signal("sig")->disconnect( slot );
    }
    {
        SignalsTestHasSignals2 obj;
        SignalsTestA srv;
        Slot< void()>::sptr slot = core::com::newSlot( &SignalsTestA::changeStatus, &srv );

        obj.signal("sig")->connect( slot );
        obj.signal< SignalsTestHasSignals2::SignalType >("sig")->emit();

        CPPUNIT_ASSERT( srv.m_val );
        obj.signal("sig")->disconnect( slot );
    }
}

//-----------------------------------------------------------------------------

} //namespace ut
} //namespace sight::core::com
