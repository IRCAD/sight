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

void equipment_test::institution_name_test()
{
    const std::string institution_name = "IRCAD";
    CPPUNIT_ASSERT(m_equipment);
    m_equipment->set_institution_name(institution_name);

    CPPUNIT_ASSERT_EQUAL(institution_name, m_equipment->get_institution_name());
}

//------------------------------------------------------------------------------

void equipment_test::equality_test()
{
    auto equipment1 = std::make_shared<data::series>();
    auto equipment2 = std::make_shared<data::series>();

    CPPUNIT_ASSERT(*equipment1 == *equipment2);

    equipment1->set_institution_name("IRCAD");
    CPPUNIT_ASSERT(*equipment1 != *equipment2);
    equipment2->set_institution_name(equipment1->get_institution_name());
    CPPUNIT_ASSERT(*equipment1 == *equipment2);

    // Test also deepcopy, just for fun
    auto dicom_series3 = std::make_shared<data::series>();
    dicom_series3->deep_copy(equipment1);
    CPPUNIT_ASSERT(*equipment1 == *dicom_series3);
}

//------------------------------------------------------------------------------

} // namespace sight::data::ut
