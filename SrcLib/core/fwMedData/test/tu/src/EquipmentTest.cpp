/************************************************************************
 *
 * Copyright (C) 2009-2015 IRCAD France
 * Copyright (C) 2012-2015 IHU Strasbourg
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
