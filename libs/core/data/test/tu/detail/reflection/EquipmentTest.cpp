/************************************************************************
 *
 * Copyright (C) 2009-2021 IRCAD France
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

#include "EquipmentTest.hpp"

#include "DataCampHelper.hpp"

#include <data/Equipment.hpp>

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION(sight::data::detail::reflection::ut::EquipmentTest);

namespace sight::data
{

namespace detail
{

namespace reflection
{

namespace ut
{

//------------------------------------------------------------------------------

void EquipmentTest::setUp()
{
}

//------------------------------------------------------------------------------

void EquipmentTest::tearDown()
{
    // Clean up after the test run.
}

//------------------------------------------------------------------------------

void EquipmentTest::propertiesTest()
{
    const std::string institution_name                        = "IHU Strasbourg / IRCAD";
    const ::DataCampHelper::PropertiesNameType dataProperties = {"fields", "institution_name"};

    data::Equipment::sptr obj = data::Equipment::New();
    obj->setInstitutionName(institution_name);

    ::DataCampHelper::visitProperties(obj->getClassname(), dataProperties);
    ::DataCampHelper::compareSimplePropertyValue(obj, "@institution_name", institution_name);
}

//------------------------------------------------------------------------------

} //namespace ut

} //namespace reflection

} //namespace detail

} //namespace sight::data
