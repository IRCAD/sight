/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2018.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "ReconstructionTest.hpp"

#include <fwData/Image.hpp>
#include <fwData/Reconstruction.hpp>

#include <boost/date_time/posix_time/posix_time.hpp>

#include <exception>
#include <iostream>
#include <map>
#include <ostream>
#include <vector>

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION( ::fwData::ut::ReconstructionTest );

namespace fwData
{
namespace ut
{

//------------------------------------------------------------------------------

void ReconstructionTest::setUp()
{
    // Set up context before running a test.
}

//------------------------------------------------------------------------------

void ReconstructionTest::tearDown()
{
    // Clean up after the test run.
}

//------------------------------------------------------------------------------

void ReconstructionTest::methode1() //test des setters et getters
{
    const bool ISVISIBLE                = true;
    const std::string CREFORGANNAME     = "OrganName";
    const std::string CREFSTRUCTURETYPE = "StructureType";

    // process
    ::fwData::Reconstruction::sptr p1 = ::fwData::Reconstruction::New();

    p1->setIsVisible( ISVISIBLE );
    p1->setOrganName( CREFORGANNAME );
    p1->setStructureType( CREFSTRUCTURETYPE );

    // check
    CPPUNIT_ASSERT_EQUAL(p1->getIsVisible(), ISVISIBLE);

    CPPUNIT_ASSERT_EQUAL(p1->getOrganName(), CREFORGANNAME);
    CPPUNIT_ASSERT_EQUAL(p1->getOrganName(), CREFORGANNAME);
    CPPUNIT_ASSERT_EQUAL(p1->getOrganName(), CREFORGANNAME);

    CPPUNIT_ASSERT_EQUAL(p1->getStructureType(), CREFSTRUCTURETYPE);
    CPPUNIT_ASSERT_EQUAL(p1->getStructureType(), CREFSTRUCTURETYPE);
    CPPUNIT_ASSERT_EQUAL(p1->getStructureType(), CREFSTRUCTURETYPE);

}

//------------------------------------------------------------------------------

void ReconstructionTest::methode2()
{
    const bool ISVISIBLE                = true;
    const std::string CREFORGANNAME     = "OrganName";
    const std::string CREFSTRUCTURETYPE = "StructureType";

    // process
    ::fwData::Reconstruction::sptr p1 = ::fwData::Reconstruction::New();

    p1->setIsVisible( ISVISIBLE );
    p1->setOrganName( CREFORGANNAME );
    p1->setStructureType( CREFSTRUCTURETYPE );

    // check
    CPPUNIT_ASSERT_EQUAL(p1->getIsVisible(), ISVISIBLE);

    CPPUNIT_ASSERT_EQUAL(p1->getOrganName(), CREFORGANNAME);
    CPPUNIT_ASSERT_EQUAL(p1->getOrganName(), CREFORGANNAME);
    CPPUNIT_ASSERT_EQUAL(p1->getOrganName(), CREFORGANNAME);

    CPPUNIT_ASSERT_EQUAL(p1->getStructureType(), CREFSTRUCTURETYPE);
    CPPUNIT_ASSERT_EQUAL(p1->getStructureType(), CREFSTRUCTURETYPE);
    CPPUNIT_ASSERT_EQUAL(p1->getStructureType(), CREFSTRUCTURETYPE);
}

//------------------------------------------------------------------------------

void ReconstructionTest::image()
{
    ::fwData::Reconstruction::sptr p1 = ::fwData::Reconstruction::New();
    ::fwData::Image::sptr i1(::fwData::Image::New());

    p1->setImage(i1);
    CPPUNIT_ASSERT_EQUAL(p1->getImage(), i1 );
}

} //namespace ut
} //namespace fwData
