/************************************************************************
 *
 * Copyright (C) 2009-2019 IRCAD France
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

#include "fwCore/base.hpp"

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION( ::fwCore::ut::DemanglerTest );

namespace fwCore
{
namespace ut
{

//------------------------------------------------------------------------------

void DemanglerTest::setUp()
{
}

//-----------------------------------------------------------------------------

class IDemanglerTest : public ::fwCore::BaseObject
{
public:
    fwCoreNonInstanciableClassDefinitionsMacro( (IDemanglerTest) )
    virtual ~IDemanglerTest();

};

IDemanglerTest::~IDemanglerTest()
{
}

//-----------------------------------------------------------------------------

class ODemanglerTest : public IDemanglerTest
{
public:
    fwCoreClassMacro(ODemanglerTest, IDemanglerTest, new ODemanglerTest);
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
    CPPUNIT_ASSERT_EQUAL(std::string("::fwCore::ut::IDemanglerTest"), ::fwCore::ut::IDemanglerTest::classname());
    CPPUNIT_ASSERT_EQUAL(std::string("::fwCore::ut::ODemanglerTest"), ::fwCore::ut::ODemanglerTest::classname());

    CPPUNIT_ASSERT_EQUAL(std::string("IDemanglerTest"), ::fwCore::ut::IDemanglerTest::leafClassname());
    CPPUNIT_ASSERT_EQUAL(std::string("ODemanglerTest"), ::fwCore::ut::ODemanglerTest::leafClassname());

    ODemanglerTest::sptr object        = std::make_shared< ODemanglerTest>();
    IDemanglerTest::sptr genericObject = object;
    CPPUNIT_ASSERT_EQUAL(std::string("::fwCore::ut::ODemanglerTest"), object->getClassname());
    CPPUNIT_ASSERT_EQUAL(std::string("::fwCore::ut::ODemanglerTest"), genericObject->getClassname());

    CPPUNIT_ASSERT_EQUAL(std::string("ODemanglerTest"), object->getLeafClassname());
    CPPUNIT_ASSERT_EQUAL(std::string("ODemanglerTest"), genericObject->getLeafClassname());
}

//-----------------------------------------------------------------------------

} //namespace ut
} //namespace fwCore
