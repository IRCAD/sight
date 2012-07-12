/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <iostream>
#include <exception>

#include <boost/thread.hpp>

#include <fwCore/util/LazyInstantiator.hpp>
#include <fwCore/util/FactoryRegistry.hpp>
#include <fwCore/mt/types.hpp>

#include "FactoryRegistryTest.hpp"


// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION( ::fwCore::ut::FactoryRegistryTest );

namespace fwCore
{
namespace ut
{

void FactoryRegistryTest::setUp()
{
    // Set up context before running a test.

}
void FactoryRegistryTest::tearDown()
{
    // Clean up after the test run.
}

//-----------------------------------------------------------------------------

class ObjectTest
{
public:
    typedef ::boost::shared_ptr< ObjectTest > sptr;

    ObjectTest() : m_name("ObjectTest")
    {
        ::fwCore::mt::ScopedLock lock(s_mutex);
        ++s_counter;
    }

    virtual ~ObjectTest()
    {}

    virtual std::string getName() const
    {
        return m_name;
    }

    std::string m_name;
    static int s_counter;
    static ::fwCore::mt::Mutex s_mutex;
};
int ObjectTest::s_counter = 0;
::fwCore::mt::Mutex ObjectTest::s_mutex;

class DerivedObjectTest : public ObjectTest
{
public:
    typedef ::boost::shared_ptr< DerivedObjectTest > sptr;

    DerivedObjectTest() : ObjectTest()
    {
        m_name = "DerivedObjectTest";
    }
};

//-----------------------------------------------------------------------------

void FactoryRegistryTest::pointerTest()
{
    ObjectTest::s_counter = 0;

    ::fwCore::util::FactoryRegistry< ObjectTest::sptr() > objectTestFactory;
    objectTestFactory.addFactory("ObjectTest", ::boost::factory<ObjectTest::sptr>());
    objectTestFactory.addFactory("DerivedObjectTest", ::boost::factory<DerivedObjectTest::sptr>());

    ObjectTest::sptr objectTest1 = objectTestFactory.create("ObjectTest");
    CPPUNIT_ASSERT_EQUAL(1, ObjectTest::s_counter);

    ObjectTest::sptr objectTest2 = objectTestFactory.create("ObjectTest");
    CPPUNIT_ASSERT_EQUAL(2, ObjectTest::s_counter);

    CPPUNIT_ASSERT_EQUAL(std::string("ObjectTest"), objectTest1->getName());
    CPPUNIT_ASSERT_EQUAL(std::string("ObjectTest"), objectTest2->getName());



    ObjectTest::sptr derivedObjectTest1 = objectTestFactory.create("DerivedObjectTest");
    CPPUNIT_ASSERT_EQUAL(3, ObjectTest::s_counter);
    ObjectTest::sptr derivedObjectTest2 = objectTestFactory.create("DerivedObjectTest");
    CPPUNIT_ASSERT_EQUAL(4, ObjectTest::s_counter);

    CPPUNIT_ASSERT_EQUAL(std::string("DerivedObjectTest"), derivedObjectTest1->getName());
    CPPUNIT_ASSERT_EQUAL(std::string("DerivedObjectTest"), derivedObjectTest2->getName());

    ObjectTest::s_counter = 0;
}

//-----------------------------------------------------------------------------

void FactoryRegistryTest::valueTest()
{
    ObjectTest::s_counter = 0;

    ::fwCore::util::FactoryRegistry< ObjectTest() > objectTestFactory;
    objectTestFactory.addFactory("ObjectTest", ::boost::value_factory<ObjectTest>());
    objectTestFactory.addFactory("DerivedObjectTest", ::boost::value_factory<DerivedObjectTest>());

    ObjectTest objectTest1 = objectTestFactory.create("ObjectTest");
    CPPUNIT_ASSERT_EQUAL(1, ObjectTest::s_counter);

    ObjectTest objectTest2 = objectTestFactory.create("ObjectTest");
    CPPUNIT_ASSERT_EQUAL(2, ObjectTest::s_counter);

    CPPUNIT_ASSERT_EQUAL(std::string("ObjectTest"), objectTest1.getName());
    CPPUNIT_ASSERT_EQUAL(std::string("ObjectTest"), objectTest2.getName());



    ObjectTest derivedObjectTest1 = objectTestFactory.create("DerivedObjectTest");
    CPPUNIT_ASSERT_EQUAL(3, ObjectTest::s_counter);
    ObjectTest derivedObjectTest2 = objectTestFactory.create("DerivedObjectTest");
    CPPUNIT_ASSERT_EQUAL(4, ObjectTest::s_counter);

    CPPUNIT_ASSERT_EQUAL(std::string("DerivedObjectTest"), derivedObjectTest1.getName());
    CPPUNIT_ASSERT_EQUAL(std::string("DerivedObjectTest"), derivedObjectTest2.getName());

    ObjectTest::s_counter = 0;
}

} //namespace ut
} //namespace fwCore
