/************************************************************************
 *
 * Copyright (C) 2009-2023 IRCAD France
 * Copyright (C) 2012-2020 IHU Strasbourg
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

#include "equipment_test.hpp"

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION(sight::data::ut::equipment_test);

namespace sight::data::ut
{

//------------------------------------------------------------------------------

void equipment_test::setUp()
{
    // Set up context before running a test.
    m_equipment = std::make_shared<data::series>();
}

//------------------------------------------------------------------------------

void equipment_test::tearDown()
{
    // Clean up after the test run.
    m_equipment.reset();
}

//------------------------------------------------------------------------------

void equipment_test::institutionNameTest()
{
    const std::string institutionName = "IRCAD";
    CPPUNIT_ASSERT(m_equipment);
    m_equipment->setInstitutionName(institutionName);

    CPPUNIT_ASSERT_EQUAL(institutionName, m_equipment->getInstitutionName());
}

//------------------------------------------------------------------------------

void equipment_test::equalityTest()
{
    auto equipment1 = std::make_shared<data::series>();
    auto equipment2 = std::make_shared<data::series>();

    CPPUNIT_ASSERT(*equipment1 == *equipment2);

    equipment1->setInstitutionName("IRCAD");
    CPPUNIT_ASSERT(*equipment1 != *equipment2);
    equipment2->setInstitutionName(equipment1->getInstitutionName());
    CPPUNIT_ASSERT(*equipment1 == *equipment2);

    // Test also deepcopy, just for fun
    auto dicomSeries3 = std::make_shared<data::series>();
    dicomSeries3->deep_copy(equipment1);
    CPPUNIT_ASSERT(*equipment1 == *dicomSeries3);
}

//------------------------------------------------------------------------------

} // namespace sight::data::ut
