/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2012.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <string>

#include <fwCom/Signal.hpp>
#include <fwCom/Signal.hxx>

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

} //namespace ut
} //namespace fwCom
