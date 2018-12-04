/************************************************************************
 *
 * Copyright (C) 2009-2018 IRCAD France
 * Copyright (C) 2012-2018 IHU Strasbourg
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
