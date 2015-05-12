/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2015.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <fwData/Image.hpp>

#include <fwDataCamp/getObject.hpp>

#include "ObjectTest.hpp"


CPPUNIT_TEST_SUITE_REGISTRATION( ::fwDataCamp::ut::ObjectTest );

namespace fwDataCamp
{
namespace ut
{

//-----------------------------------------------------------------------------

// Set up context before running a test.
void ObjectTest::setUp()
{
}

//-----------------------------------------------------------------------------

void ObjectTest::tearDown()
{
    // Clean up after the test run.
}

//-----------------------------------------------------------------------------

void ObjectTest::callFunctionTest()
{
    ::fwData::Image::sptr imgage = ::fwData::Image::New();
    ::camp::UserObject campObj(imgage.get());
    const ::camp::Class & metaClass = campObj.getClass();
    CPPUNIT_ASSERT(metaClass.hasFunction("classname"));
    CPPUNIT_ASSERT(metaClass.hasFunction("is_a"));

    CPPUNIT_ASSERT(campObj.call("classname").to<std::string>() == "::fwData::Image");
    CPPUNIT_ASSERT(campObj.call("is_a", ::camp::Args("::fwData::Object")).to<bool>());
    CPPUNIT_ASSERT(campObj.call("is_a", ::camp::Args("::fwData::Image")).to<bool>());
    CPPUNIT_ASSERT(!campObj.call("is_a", ::camp::Args("::fwData::Float")).to<bool>());

}

//-----------------------------------------------------------------------------

}  // namespace ut
}  // namespace fwDataCamp
