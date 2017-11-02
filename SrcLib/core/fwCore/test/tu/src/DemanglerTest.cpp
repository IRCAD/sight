/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

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
    fwCoreClassDefinitionsWithFactoryMacro( (ODemanglerTest)(IDemanglerTest), (()), new ODemanglerTest)
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
