/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <iostream>
#include <exception>
#include <vector>
#include <ostream>
#include <map>
#include <boost/date_time/posix_time/posix_time.hpp>

#include <fwData/Edge.hpp>
#include "EdgeTest.hpp"


// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION( ::fwData::ut::EdgeTest );

namespace fwData
{
namespace ut
{

void EdgeTest::setUp()
{
    // Set up context before running a test.

}
void EdgeTest::tearDown()
{
    // Clean up after the test run.
}

void EdgeTest::methode1()
{
    //-----------test values
    const std::string STR1 = "toto";
    const std::string STR2 = "titi";
    const std::string STR3 = ::fwData::Edge::NATURE_FLOW;
    ::fwData::Edge::NewSptr edge;

    edge->setIdentifiers(STR1, STR2);
    edge->setNature(STR3);

    CPPUNIT_ASSERT_EQUAL(edge->getFromPortID(), STR1);
    CPPUNIT_ASSERT_EQUAL(edge->getToPortID(), STR2);
    CPPUNIT_ASSERT_EQUAL(edge->getNature(), STR3);
}

} //namespace ut
} //namespace fwData

