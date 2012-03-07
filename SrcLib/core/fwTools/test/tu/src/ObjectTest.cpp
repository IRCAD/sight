/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <fwTools/Object.hpp>

#include "ObjectTest.hpp"

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION( ::fwTools::ut::ObjectTest );

namespace fwTools
{
namespace ut
{

void ObjectTest::setUp()
{
    // Set up context before running a test.
}

void ObjectTest::tearDown()
{
    // Clean up after the test run.
}

void ObjectTest::idTest()
{
    const std::string UUID = "uid" ;
    const std::string FIELDID = "fieldID";

    // process
    ::fwTools::Object::NewSptr obj;
    ::fwTools::Object::NewSptr obj2;
    ::fwTools::Field::sptr field2;
    field2 = obj->setFieldSingleElement(FIELDID,  obj2);

    obj->setID(UUID);

    // check
    CPPUNIT_ASSERT(obj->hasID());
    CPPUNIT_ASSERT_EQUAL(obj->getID(), UUID);
    CPPUNIT_ASSERT(obj->getFieldSize(FIELDID));
    CPPUNIT_ASSERT_EQUAL(obj->getField(FIELDID), field2);

    obj->removeField(FIELDID);
    CPPUNIT_ASSERT(obj->getFieldSize(FIELDID) == false);
}

} // namespace ut
} // namespace fwTools
