/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2012.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <string>

#include <boost/thread.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>

#include <fwCom/Signal.hpp>
#include <fwCom/Signal.hxx>

#include <fwCom/Slot.hpp>
#include <fwCom/Slot.hxx>

#include <fwThread/Worker.hpp>

#include "SignalTest.hpp"


// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION( ::fwCom::ut::SignalTest );

namespace fwCom
{
namespace ut
{

void SignalTest::setUp()
{
    // Set up context before running a test.

}
void SignalTest::tearDown()
{
    // Clean up after the test run.
}

//-----------------------------------------------------------------------------

void SignalTest::buildTest ()
{
    {
        typedef void Signature();
        ::fwCom::Signal< Signature >::sptr sig = ::fwCom::Signal< Signature >::New();
        CPPUNIT_ASSERT( sig );
    }

    {
        typedef void Signature(int);
        ::fwCom::Signal< Signature >::sptr sig = ::fwCom::Signal< Signature >::New();
        CPPUNIT_ASSERT( sig );
    }

    {
        typedef void Signature(char, float);
        ::fwCom::Signal< Signature >::sptr sig = ::fwCom::Signal< Signature >::New();
        CPPUNIT_ASSERT( sig );
    }

    {
        typedef void Signature(short, double, std::string);
        ::fwCom::Signal< Signature >::sptr sig = ::fwCom::Signal< Signature >::New();
        CPPUNIT_ASSERT( sig );
    }
}

//-----------------------------------------------------------------------------

struct SignalTestClass
{
    SignalTestClass() :
        m_method0(false),
        m_method1(false),
        m_method2(false),
        m_method3(false)
    {}

    void method0(){ m_method0 = true; }
    void method00(){ m_method00 = true; }
    float method1(float f){
        m_method1 = true;
        return 2*f;
    }

    float method2(float f, int i){
        m_method2 = true;
        return 2*f;
    }

    float method3(float f, double d, std::string s){
        m_method3 = true;
        return 2*f;
    }

    bool m_method0;
    bool m_method00;
    bool m_method1;
    bool m_method2;
    bool m_method3;

};

//-----------------------------------------------------------------------------

void SignalTest::connectTest()
{
    SignalTestClass testObject;
    ::fwCom::Slot< void() >::sptr slot0
        = ::fwCom::newSlot(&SignalTestClass::method0, &testObject);
    ::fwCom::Slot< float(float) >::sptr slot1
        = ::fwCom::newSlot(&SignalTestClass::method1, &testObject);
    ::fwCom::Slot< float(float, int) >::sptr slot2
        = ::fwCom::newSlot(&SignalTestClass::method2, &testObject);
    ::fwCom::Slot< float(float, double, std::string) >::sptr slot3
        = ::fwCom::newSlot(&SignalTestClass::method3, &testObject);

    ::fwCom::Connection connection;
    CPPUNIT_ASSERT(connection.expired());

    {
        typedef void Signature();
        ::fwCom::Signal< Signature >::sptr sig = ::fwCom::Signal< Signature >::New();

        connection = sig->connect(slot0);
        CPPUNIT_ASSERT(!connection.expired());
        CPPUNIT_ASSERT_EQUAL((size_t)1, sig->getNumberOfConnections());
    }

    CPPUNIT_ASSERT(connection.expired());

    {
        typedef void Signature(float);
        ::fwCom::Signal< Signature >::sptr sig = ::fwCom::Signal< Signature >::New();

        connection = sig->connect(slot1);
        CPPUNIT_ASSERT(!connection.expired());
        CPPUNIT_ASSERT_EQUAL((size_t)1, sig->getNumberOfConnections());
    }

    CPPUNIT_ASSERT(connection.expired());

    {
        typedef void Signature(float, int);
        ::fwCom::Signal< Signature >::sptr sig = ::fwCom::Signal< Signature >::New();

        connection = sig->connect(slot2);
        CPPUNIT_ASSERT(!connection.expired());
        CPPUNIT_ASSERT_EQUAL((size_t)1, sig->getNumberOfConnections());
    }

    CPPUNIT_ASSERT(connection.expired());

    {
        typedef void Signature(float, double, std::string);
        ::fwCom::Signal< Signature >::sptr sig = ::fwCom::Signal< Signature >::New();

        connection = sig->connect(slot3);
        CPPUNIT_ASSERT(!connection.expired());
        CPPUNIT_ASSERT_EQUAL((size_t)1, sig->getNumberOfConnections());
    }

    CPPUNIT_ASSERT(connection.expired());

    // Diconnect
    {
        typedef void Signature();
        ::fwCom::Signal< Signature >::sptr sig = ::fwCom::Signal< Signature >::New();

        connection = sig->connect(slot0);
        CPPUNIT_ASSERT(!connection.expired());
        CPPUNIT_ASSERT_EQUAL((size_t)1, sig->getNumberOfConnections());
        connection.disconnect();
        CPPUNIT_ASSERT(connection.expired());
        CPPUNIT_ASSERT_EQUAL((size_t)0, sig->getNumberOfConnections());
    }

    {
        typedef void Signature();
        ::fwCom::Signal< Signature >::sptr sig = ::fwCom::Signal< Signature >::New();

        connection = sig->connect(slot0);
        CPPUNIT_ASSERT(!connection.expired());
        CPPUNIT_ASSERT_EQUAL((size_t)1, sig->getNumberOfConnections());
        sig->disconnect(slot0);
        CPPUNIT_ASSERT(connection.expired());
        CPPUNIT_ASSERT_EQUAL((size_t)0, sig->getNumberOfConnections());
    }

    {
        typedef void Signature();
        ::fwCom::Signal< Signature >::sptr sig = ::fwCom::Signal< Signature >::New();

        ::fwCom::Slot< void() >::sptr slot
            = ::fwCom::newSlot(&SignalTestClass::method0, &testObject);

        connection = sig->connect(slot0);
        ::fwCom::Connection connection = sig->connect(slot);

        CPPUNIT_ASSERT(!connection.expired());
        CPPUNIT_ASSERT_EQUAL((size_t)2, sig->getNumberOfConnections());
        sig->disconnectAll();
        CPPUNIT_ASSERT(connection.expired());
        CPPUNIT_ASSERT_EQUAL((size_t)0, sig->getNumberOfConnections());
    }
}

//-----------------------------------------------------------------------------

void SignalTest::emitTest()
{
    ::fwCom::Connection connection;

    {
        typedef void Signature();
        SignalTestClass testObject;

        ::fwCom::Slot< Signature >::sptr slot
            = ::fwCom::newSlot(&SignalTestClass::method0, &testObject);

        ::fwCom::Signal< Signature >::sptr sig = ::fwCom::Signal< Signature >::New();

        connection = sig->connect(slot);
        CPPUNIT_ASSERT(!connection.expired());

        CPPUNIT_ASSERT_EQUAL((size_t)1, sig->getNumberOfConnections());
        sig->emit();
        CPPUNIT_ASSERT(testObject.m_method0);
    }

    CPPUNIT_ASSERT(connection.expired());

    {
        typedef void Signature(float);
        SignalTestClass testObject;

        ::fwCom::Slot< float(float) >::sptr slot
            = ::fwCom::newSlot(&SignalTestClass::method1, &testObject);

        ::fwCom::Signal< Signature >::sptr sig = ::fwCom::Signal< Signature >::New();

        connection = sig->connect(slot);
        CPPUNIT_ASSERT(!connection.expired());

        CPPUNIT_ASSERT_EQUAL((size_t)1, sig->getNumberOfConnections());

        sig->emit(21.0f);
        CPPUNIT_ASSERT(testObject.m_method1);
    }

    CPPUNIT_ASSERT(connection.expired());

    {
        typedef void Signature(float, int);
        SignalTestClass testObject;

        ::fwCom::Slot< float(float, int) >::sptr slot
            = ::fwCom::newSlot(&SignalTestClass::method2, &testObject);

        ::fwCom::Signal< Signature >::sptr sig = ::fwCom::Signal< Signature >::New();

        connection = sig->connect(slot);
        CPPUNIT_ASSERT(!connection.expired());

        CPPUNIT_ASSERT_EQUAL((size_t)1, sig->getNumberOfConnections());

        sig->emit(21.0f, 42);
        CPPUNIT_ASSERT(testObject.m_method2);
    }

    CPPUNIT_ASSERT(connection.expired());

    {
        typedef void Signature(float, double, std::string);
        SignalTestClass testObject;

        ::fwCom::Slot< float(float, double, std::string) >::sptr slot
            = ::fwCom::newSlot(&SignalTestClass::method3, &testObject);

        ::fwCom::Signal< Signature >::sptr sig = ::fwCom::Signal< Signature >::New();

        connection = sig->connect(slot);
        CPPUNIT_ASSERT(!connection.expired());

        CPPUNIT_ASSERT_EQUAL((size_t)1, sig->getNumberOfConnections());

        sig->emit(21.0f, 42.0, "emit");
        CPPUNIT_ASSERT(testObject.m_method3);
    }

    CPPUNIT_ASSERT(connection.expired());
}

//-----------------------------------------------------------------------------

void SignalTest::autoSlotDisconnectTest()
{
    typedef void Signature();
    SignalTestClass testObject;

    ::fwCom::Signal< Signature >::sptr sig = ::fwCom::Signal< Signature >::New();

    {
        ::fwCom::Slot< void() >::sptr slot0
            = ::fwCom::newSlot(&SignalTestClass::method0, &testObject);

        sig->connect(slot0);
        CPPUNIT_ASSERT_EQUAL((size_t)1, sig->getNumberOfConnections());

        {
            ::fwCom::Slot< void() >::sptr slot00
                = ::fwCom::newSlot(&SignalTestClass::method00, &testObject);

            sig->connect(slot00);
            CPPUNIT_ASSERT_EQUAL((size_t)2, sig->getNumberOfConnections());
        }

        CPPUNIT_ASSERT_EQUAL((size_t)1, sig->getNumberOfConnections());
    }

    CPPUNIT_ASSERT_EQUAL((size_t)0, sig->getNumberOfConnections());

}

//-----------------------------------------------------------------------------

void SignalTest::argumentLossTest()
{
    typedef void Signature(float, double, std::string);
    SignalTestClass testObject;

    ::fwCom::Signal< Signature >::sptr sig = ::fwCom::Signal< Signature >::New();

    ::fwCom::Slot< void() >::sptr slot0
        = ::fwCom::newSlot(&SignalTestClass::method0, &testObject);
    ::fwCom::Slot< float(float) >::sptr slot1
        = ::fwCom::newSlot(&SignalTestClass::method1, &testObject);
    ::fwCom::Slot< float(float, double, std::string) >::sptr slot3
        = ::fwCom::newSlot(&SignalTestClass::method3, &testObject);

    // sig->connect(slot0); // FIXME
    // CPPUNIT_ASSERT_EQUAL((size_t)1, sig->getNumberOfConnections());

    //sig->connect(slot1); // FIXME
    //CPPUNIT_ASSERT_EQUAL((size_t)2, sig->getNumberOfConnections());

    sig->connect(slot3);
    //CPPUNIT_ASSERT_EQUAL((size_t)3, sig->getNumberOfConnections());   // see FIXME
    CPPUNIT_ASSERT_EQUAL((size_t)1, sig->getNumberOfConnections());

    sig->emit(21.0f, 42.0, "emit");

    //CPPUNIT_ASSERT(testObject.m_method0); // see FIXME
    //CPPUNIT_ASSERT(testObject.m_method1); // see FIXME
    CPPUNIT_ASSERT(testObject.m_method3);

    sig->disconnectAll();
    CPPUNIT_ASSERT_EQUAL((size_t)0, sig->getNumberOfConnections());
}

//-----------------------------------------------------------------------------

void SignalTest::asyncEmitTest()
{
    ::fwCom::Connection connection;
    ::fwThread::Worker::sptr worker = ::fwThread::Worker::New();

    {
        typedef void Signature();
        SignalTestClass testObject;

        ::fwCom::Slot< Signature >::sptr slot
            = ::fwCom::newSlot(&SignalTestClass::method0, &testObject);
        slot->setWorker(worker);

        ::fwCom::Signal< Signature >::sptr sig = ::fwCom::Signal< Signature >::New();

        connection = sig->connect(slot);
        CPPUNIT_ASSERT(!connection.expired());

        CPPUNIT_ASSERT_EQUAL((size_t)1, sig->getNumberOfConnections());
        sig->asyncEmit();
        ::boost::this_thread::sleep( ::boost::posix_time::seconds(1));

        CPPUNIT_ASSERT(testObject.m_method0);
    }

    CPPUNIT_ASSERT(connection.expired());

    {
        typedef void Signature(float);
        SignalTestClass testObject;

        ::fwCom::Slot< float(float) >::sptr slot
            = ::fwCom::newSlot(&SignalTestClass::method1, &testObject);
        slot->setWorker(worker);

        ::fwCom::Signal< Signature >::sptr sig = ::fwCom::Signal< Signature >::New();

        connection = sig->connect(slot);
        CPPUNIT_ASSERT(!connection.expired());

        CPPUNIT_ASSERT_EQUAL((size_t)1, sig->getNumberOfConnections());

        sig->asyncEmit(21.0f);
        ::boost::this_thread::sleep( ::boost::posix_time::seconds(1));

        CPPUNIT_ASSERT(testObject.m_method1);
    }

    CPPUNIT_ASSERT(connection.expired());

    {
        typedef void Signature(float, int);
        SignalTestClass testObject;

        ::fwCom::Slot< float(float, int) >::sptr slot
            = ::fwCom::newSlot(&SignalTestClass::method2, &testObject);
        slot->setWorker(worker);

        ::fwCom::Signal< Signature >::sptr sig = ::fwCom::Signal< Signature >::New();

        connection = sig->connect(slot);
        CPPUNIT_ASSERT(!connection.expired());

        CPPUNIT_ASSERT_EQUAL((size_t)1, sig->getNumberOfConnections());

        sig->asyncEmit(21.0f, 42);
        ::boost::this_thread::sleep( ::boost::posix_time::seconds(1));

        CPPUNIT_ASSERT(testObject.m_method2);
    }

    CPPUNIT_ASSERT(connection.expired());

    {
        typedef void Signature(float, double, std::string);
        SignalTestClass testObject;

        ::fwCom::Slot< float(float, double, std::string) >::sptr slot
            = ::fwCom::newSlot(&SignalTestClass::method3, &testObject);
        slot->setWorker(worker);

        ::fwCom::Signal< Signature >::sptr sig = ::fwCom::Signal< Signature >::New();

        connection = sig->connect(slot);
        CPPUNIT_ASSERT(!connection.expired());

        CPPUNIT_ASSERT_EQUAL((size_t)1, sig->getNumberOfConnections());

        sig->asyncEmit(21.0f, 42.0, "emit");
        ::boost::this_thread::sleep( ::boost::posix_time::seconds(1));

        CPPUNIT_ASSERT(testObject.m_method3);
    }

    CPPUNIT_ASSERT(connection.expired());
}

} //namespace ut
} //namespace fwCom
