/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2015.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */


#include "EquipmentTest.hpp"

#include "fwMedData/Equipment.hpp"


// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION( ::fwMedData::ut::EquipmentTest );

namespace fwMedData
{
namespace ut
{

//------------------------------------------------------------------------------

void EquipmentTest::setUp()
{
    // Set up context before running a test.
    m_equipment = ::fwMedData::Equipment::New();
}

//------------------------------------------------------------------------------

void EquipmentTest::tearDown()
{
    // Clean up after the test run.
    m_equipment.reset();
}

//------------------------------------------------------------------------------

void EquipmentTest::institutionNameTest()
{
    const std::string institutionName = "Ircad / IHU";
    CPPUNIT_ASSERT(m_equipment);
    m_equipment->setInstitutionName(institutionName);
    CPPUNIT_ASSERT_EQUAL(institutionName, m_equipment->getInstitutionName());
}

//------------------------------------------------------------------------------

} //namespace ut
} //namespace fwMedData
