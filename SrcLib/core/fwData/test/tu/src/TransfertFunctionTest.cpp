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

#include "fwData/TransfertFunction.hpp"
#include "fwData/Color.hpp"
#include "TransfertFunctionTest.hpp"


// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION( TransfertFunctionTest );

void TransfertFunctionTest::setUp()
{
    // Set up context before running a test.

}
void TransfertFunctionTest::tearDown()
{
    // Clean up after the test run.
}

void TransfertFunctionTest::methode1()
{
    //-----------test values
    const ::fwData::Color::sptr C1(new ::fwData::Color(0.5f, 0.4f ,0.6f, 0.1f));
    const ::fwData::Color::sptr C2(new ::fwData::Color());
    const int INDEX1 = 2;
    const int INDEX2 = 5;
    const std::string NAME = "tfName";
    const std::string ENCODING = "tfencoding";
    const boost::uint32_t SIZE = 2;

    ::fwData::TransfertFunction::NewSptr tf;

    tf->setCRefEncoding(ENCODING);
    tf->setCRefName(NAME);
    *( tf->getColor(INDEX1) ) = *C1;
    *( tf->getColor(INDEX2) ) = *C2;

    CPPUNIT_ASSERT_EQUAL(tf->getTransfertFunctionPointSize(), SIZE);
    CPPUNIT_ASSERT_EQUAL(tf->getName(), NAME);
    CPPUNIT_ASSERT_EQUAL(tf->getEncoding(), ENCODING);
    CPPUNIT_ASSERT_EQUAL(tf->getColor(INDEX1)->getRGBA()[0], C1->getRGBA()[0]);
    CPPUNIT_ASSERT_EQUAL(tf->getColor(INDEX1)->getRGBA()[1], C1->getRGBA()[1]);
}



