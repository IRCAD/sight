/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <fwData/Region.hpp>
#include "RegionTest.hpp"

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION(RegionTest );

void RegionTest::setUp()
{
    // Set up context before running a test.
}

//------------------------------------------------------------------------------

void RegionTest::tearDown()
{
    // Clean up after the test run.
}

//------------------------------------------------------------------------------

void RegionTest::methode1()
{
    ::boost::uint8_t DIM    = 14 ;
    std::vector<double> vOrigin;
    vOrigin = std::vector<double>(DIM, 0) ;
    //vOrigin.push_back( DIM ) ;

    // process
    ::fwData::Region::NewSptr p1(DIM);

    // check
    CPPUNIT_ASSERT_EQUAL(p1->getRefOrigin().back(), vOrigin.back());

}
