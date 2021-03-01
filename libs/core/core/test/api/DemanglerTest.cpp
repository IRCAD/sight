/************************************************************************
 *
 * Copyright (C) 2009-2021 IRCAD France
 * Copyright (C) 2012-2019 IHU Strasbourg
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

#include "DemanglerTest.hpp"

#include "core/base.hpp"

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION( sight::core::ut::DemanglerTest );

namespace sight::core
{
namespace ut
{

//------------------------------------------------------------------------------

void DemanglerTest::setUp()
{
}

//-----------------------------------------------------------------------------

class IDemanglerTest : public core::BaseObject
{
public:
    SIGHT_DECLARE_CLASS(IDemanglerTest)
    virtual ~IDemanglerTest();

};

IDemanglerTest::~IDemanglerTest()
{
}

//-----------------------------------------------------------------------------

class ODemanglerTest : public IDemanglerTest
{
public:
    SIGHT_DECLARE_CLASS(ODemanglerTest, IDemanglerTest, new ODemanglerTest)
    virtual ~ODemanglerTest();
};

ODemanglerTest::~ODemanglerTest()
{
}

//-----------------------------------------------------------------------------

void DemanglerTest::tearDown()
{
}

//-----------------------------------------------------------------------------

void DemanglerTest::classNameTest()
{
    CPPUNIT_ASSERT_EQUAL(std::string("::sight::core::ut::IDemanglerTest"), core::ut::IDemanglerTest::classname());
    CPPUNIT_ASSERT_EQUAL(std::string("::sight::core::ut::ODemanglerTest"), core::ut::ODemanglerTest::classname());

    CPPUNIT_ASSERT_EQUAL(std::string("IDemanglerTest"), core::ut::IDemanglerTest::leafClassname());
    CPPUNIT_ASSERT_EQUAL(std::string("ODemanglerTest"), core::ut::ODemanglerTest::leafClassname());

    ODemanglerTest::sptr object        = std::make_shared< ODemanglerTest>();
    IDemanglerTest::sptr genericObject = object;
    CPPUNIT_ASSERT_EQUAL(std::string("::sight::core::ut::ODemanglerTest"), object->getClassname());
    CPPUNIT_ASSERT_EQUAL(std::string("::sight::core::ut::ODemanglerTest"), genericObject->getClassname());

    CPPUNIT_ASSERT_EQUAL(std::string("ODemanglerTest"), object->getLeafClassname());
    CPPUNIT_ASSERT_EQUAL(std::string("ODemanglerTest"), genericObject->getLeafClassname());
}

//-----------------------------------------------------------------------------

} //namespace ut
} //namespace sight::core
