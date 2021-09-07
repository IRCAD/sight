/************************************************************************
 *
 * Copyright (C) 2017-2021 IRCAD France
 * Copyright (C) 2017-2021 IHU Strasbourg
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

#include "LandmarksTest.hpp"

#include "DataCampHelper.hpp"

#include <data/Landmarks.hpp>
#include <data/String.hpp>

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION(sight::data::reflection::ut::LandmarksTest);

namespace sight::data
{

namespace reflection
{

namespace ut
{

//------------------------------------------------------------------------------

void LandmarksTest::setUp()
{
}

//------------------------------------------------------------------------------

void LandmarksTest::tearDown()
{
    // Clean up after the test run.
}

//------------------------------------------------------------------------------

void LandmarksTest::propertiesTest()
{
    const std::string VALUE_NAME                        = "myValueName";
    const std::string VALUE                             = "myValue";
    ::DataCampHelper::PropertiesNameType dataProperties = {"fields"};

    data::Landmarks::sptr obj = data::Landmarks::New();
    data::String::sptr str    = data::String::New(VALUE);
    obj->setField(VALUE_NAME, str);

    ::DataCampHelper::visitProperties(obj->getClassname(), dataProperties);
    ::DataCampHelper::compareSimplePropertyValue(obj, "@fields.myValueName", VALUE);
}

//------------------------------------------------------------------------------

} //namespace ut

} //namespace reflection

} //namespace sight::dataCamp
