/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2015.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <boost/foreach.hpp>

#include <fwCom/HasSignals.hpp>
#include <fwCom/Signals.hpp>
#include <fwCom/Signal.hpp>
#include <fwCom/Signal.hxx>

#include "SignalsTest.hpp"


// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION( ::fwCom::ut::SignalsTest );

namespace fwCom
{
namespace ut
{

void SignalsTest::setUp()
{
    // Set up context before running a test.

}
void SignalsTest::tearDown()
{
    // Clean up after the test run.
}

//-----------------------------------------------------------------------------

void SignalsTest::buildTest()
{
    ::fwCom::Signals signals;

    ::fwCom::Signal< void() >::sptr sig = ::fwCom::Signal< void() >::New();
    CPPUNIT_ASSERT( sig );

    typedef void Signature (int, float);
    ::fwCom::Signal< Signature >::sptr sig2 = ::fwCom::Signal< Signature >::New();
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
    typedef ::fwCom::Signal< void ()> SignalType;

    SignalsTestHasSignals()
    {
        SignalType::sptr signal = SignalType::New();
        HasSignals::m_signals("sig", signal);
#ifdef COM_LOG
        signal->setID("sig");
#endif
    }
};

struct SignalsTestA
{

    SignalsTestA()
    {
        m_val = false;
    }

    void changeStatus()
    {
        m_val = !m_val;
    }

    bool m_val;
};
//-----------------------------------------------------------------------------

void SignalsTest::hasSignalsTest()
{
    SignalsTestHasSignals obj;
    SignalsTestA srv;
    Slot< void()>::sptr slot = ::fwCom::newSlot( &SignalsTestA::changeStatus, &srv );
#ifdef COM_LOG
    slot->setID("changeStatus");
#endif
    obj.signal("sig")->connect( slot );
    obj.signal< SignalsTestHasSignals::SignalType >("sig")->emit();

    CPPUNIT_ASSERT( srv.m_val );
    obj.signal("sig")->disconnect( slot );
}

//-----------------------------------------------------------------------------

void SignalsTest::signalsIDTest()
{
#ifdef COM_LOG
    ::fwCom::Signals signals;

    ::fwCom::Signal< void() >::sptr sig1 = ::fwCom::Signal< void() >::New();

    typedef void Signature (int, float);
    ::fwCom::Signal< Signature >::sptr sig2 = ::fwCom::Signal< Signature >::New();

    signals("sig1", sig1)
        ("sig2", sig2);

    signals.setID();
    CPPUNIT_ASSERT_EQUAL( std::string("sig1"), sig1->getID() );
    CPPUNIT_ASSERT_EQUAL( std::string("sig2"), sig2->getID() );

    signals.setID("prefix::");
    CPPUNIT_ASSERT_EQUAL( std::string("prefix::sig1"), sig1->getID() );
    CPPUNIT_ASSERT_EQUAL( std::string("prefix::sig2"), sig2->getID() );
#endif
}

//-----------------------------------------------------------------------------

} //namespace ut
} //namespace fwCom
