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

    // process
    ::fwTools::Object::NewSptr obj;

    obj->setID(UUID);

    // check
    CPPUNIT_ASSERT(obj->hasID());
    CPPUNIT_ASSERT_EQUAL(obj->getID(), UUID);
}

} // namespace ut
} // namespace fwTools
