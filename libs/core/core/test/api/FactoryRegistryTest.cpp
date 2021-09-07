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

#include "FactoryRegistryTest.hpp"

#include <core/FactoryRegistry.hpp>
#include <core/LazyInstantiator.hpp>
#include <core/mt/types.hpp>
#include <core/spyLog.hpp>

#include <chrono>
#include <exception>
#include <functional>
#include <iostream>
#include <thread>

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION(sight::core::ut::FactoryRegistryTest);

namespace sight::core
{

namespace ut
{

//------------------------------------------------------------------------------

void FactoryRegistryTest::setUp()
{
    // Set up context before running a test.
}

//------------------------------------------------------------------------------

void FactoryRegistryTest::tearDown()
{
    // Clean up after the test run.
}

//-----------------------------------------------------------------------------

class ObjectTest
{
public:

    typedef std::shared_ptr<ObjectTest> sptr;

    ObjectTest() :
        m_name("ObjectTest")
    {
        core::mt::ScopedLock lock(s_mutex);
        ++s_counter;
    }

    ObjectTest(const std::string& name) :
        m_name(name)
    {
        core::mt::ScopedLock lock(s_mutex);
        ++s_counter;
    }

    ObjectTest(int msec) :
        m_name("ObjectTest+sleep")
    {
        core::mt::ScopedLock lock(s_mutex);
        std::this_thread::sleep_for(std::chrono::milliseconds(msec));
        ++s_counter;
    }

    virtual ~ObjectTest()
    {
    }

    //------------------------------------------------------------------------------

    virtual std::string getName() const
    {
        return m_name;
    }

    std::string m_name;
    static int s_counter;
    static core::mt::Mutex s_mutex;
};

int ObjectTest::s_counter = 0;
core::mt::Mutex ObjectTest::s_mutex;

class DerivedObjectTest : public ObjectTest
{
public:

    typedef std::shared_ptr<DerivedObjectTest> sptr;

    DerivedObjectTest() :
        ObjectTest()
    {
        m_name = "DerivedObjectTest";
    }

    DerivedObjectTest(const std::string& name) :
        ObjectTest(name)
    {
    }

    DerivedObjectTest(int msec) :
        ObjectTest(msec)
    {
    }
};

//-----------------------------------------------------------------------------

void FactoryRegistryTest::pointerTest()
{
    ObjectTest::s_counter = 0;

    typedef core::FactoryRegistry<ObjectTest::sptr()> FactoryType;
    FactoryType objectTestFactory;
    objectTestFactory.addFactory(
        "ObjectTest",
        []() -> ObjectTest::sptr
            {
                return std::make_shared<ObjectTest>();
            });
    objectTestFactory.addFactory(
        "DerivedObjectTest",
        []() -> DerivedObjectTest::sptr
            {
                return std::make_shared<DerivedObjectTest>();
            });

    FactoryType::KeyVectorType keys = {"ObjectTest", "DerivedObjectTest"};
    std::sort(keys.begin(), keys.end());
    FactoryType::KeyVectorType vectKeys = objectTestFactory.getFactoryKeys();
    std::sort(vectKeys.begin(), vectKeys.end());
    CPPUNIT_ASSERT(keys == vectKeys);

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

    core::FactoryRegistry<ObjectTest()> objectTestFactory;
    objectTestFactory.addFactory(
        "ObjectTest",
        []() -> ObjectTest
            {
                return ObjectTest();
            });
    objectTestFactory.addFactory(
        "DerivedObjectTest",
        []() -> DerivedObjectTest
            {
                return DerivedObjectTest();
            });

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

//-----------------------------------------------------------------------------

void FactoryRegistryTest::argTest()
{
    ObjectTest::s_counter = 0;

    core::FactoryRegistry<ObjectTest::sptr(std::string)> objectTestFactory;
    objectTestFactory.addFactory(
        "ObjectTest",
        [](const std::string& name) -> ObjectTest::sptr
            {
                return std::make_shared<ObjectTest>(name);
            });
    objectTestFactory.addFactory(
        "DerivedObjectTest",
        [](const std::string& name) -> DerivedObjectTest::sptr
            {
                return std::make_shared<DerivedObjectTest>(name);
            });

    std::string objTest1("ObjectTest1");
    std::string objTest2("ObjectTest2");
    ObjectTest::sptr objectTest1 = objectTestFactory.create("ObjectTest", objTest1);
    ObjectTest::sptr objectTest2 = objectTestFactory.create("ObjectTest", objTest2);
    CPPUNIT_ASSERT_EQUAL(std::string("ObjectTest1"), objectTest1->getName());
    CPPUNIT_ASSERT_EQUAL(std::string("ObjectTest2"), objectTest2->getName());

    ObjectTest::sptr derivedObjectTest1;
    ObjectTest::sptr derivedObjectTest2;
    std::string derObjTest1("DerivedObjectTest1");
    std::string derObjTest2("DerivedObjectTest2");
    derivedObjectTest1 = objectTestFactory.create("DerivedObjectTest", derObjTest1);
    derivedObjectTest2 = objectTestFactory.create("DerivedObjectTest", derObjTest2);
    CPPUNIT_ASSERT_EQUAL(std::string("DerivedObjectTest1"), derivedObjectTest1->getName());
    CPPUNIT_ASSERT_EQUAL(std::string("DerivedObjectTest2"), derivedObjectTest2->getName());

    CPPUNIT_ASSERT_EQUAL(4, ObjectTest::s_counter);

    ObjectTest::s_counter = 0;
}

//-----------------------------------------------------------------------------

typedef core::FactoryRegistry<ObjectTest::sptr(int)> ThreadSafetyTestFactoryType;

struct UseFactoryThread
{
    typedef std::shared_ptr<UseFactoryThread> sptr;
    typedef std::vector<ObjectTest::sptr> ObjectVectorType;

    UseFactoryThread(const ThreadSafetyTestFactoryType& factory, std::string objType = "ObjectTest") :
        m_factory(factory),
        m_objectType(objType)
    {
    }

    //------------------------------------------------------------------------------

    void run()
    {
        int duration = 20;
        for(int i = 0 ; i < s_NBOBJECTS ; ++i)
        {
            SIGHT_WARN("building 1 " << m_objectType << "... ");
            m_objects.push_back(m_factory.create(m_objectType, duration));
            SIGHT_WARN("built 1 " << m_objectType << "... ");
        }
    }

    const ThreadSafetyTestFactoryType& m_factory;
    ObjectVectorType m_objects;
    std::string m_objectType;
    static const int s_NBOBJECTS;
};

const int UseFactoryThread::s_NBOBJECTS = 10;

struct PopulateRegistryThread
{
    typedef std::shared_ptr<PopulateRegistryThread> sptr;
    typedef std::vector<ObjectTest::sptr> ObjectVectorType;

    PopulateRegistryThread(ThreadSafetyTestFactoryType& factory) :
        m_factory(factory)
    {
    }

    //------------------------------------------------------------------------------

    void run()
    {
        for(int i = 0 ; i < s_NBREGISTRYITEMS ; ++i)
        {
            std::stringstream ss;
            ss << "PopulateFactoryThreadObject-" << std::this_thread::get_id() << "-" << i;
            std::string name = ss.str();
            SIGHT_WARN("adding " + name + "... ");
            m_factory.addFactory(
                name,
                [](int msec) -> ObjectTest::sptr
                    {
                        return std::make_shared<ObjectTest>(msec);
                    });
            SIGHT_WARN("added " + name + "... ");
            std::this_thread::sleep_for(std::chrono::milliseconds(1));
        }
    }

    ThreadSafetyTestFactoryType& m_factory;
    static const int s_NBREGISTRYITEMS;
};

const int PopulateRegistryThread::s_NBREGISTRYITEMS = 1000;

//------------------------------------------------------------------------------

void FactoryRegistryTest::threadSafetyTest()
{
    ObjectTest::s_counter = 0;

    ThreadSafetyTestFactoryType objectTestFactory;
    objectTestFactory.addFactory(
        "ObjectTest",
        [](int msec) -> ObjectTest::sptr
            {
                return std::make_shared<ObjectTest>(msec);
            });
    objectTestFactory.addFactory(
        "DerivedObjectTest",
        [](int msec) -> DerivedObjectTest::sptr
            {
                return std::make_shared<DerivedObjectTest>(msec);
            });

    const int NB_THREAD(10);

    typedef std::vector<UseFactoryThread::sptr> UseFactoryThreadVector;
    std::vector<std::thread> tg;

    UseFactoryThreadVector objects;
    for(size_t i = 0 ; i < NB_THREAD ; i++)
    {
        UseFactoryThread::sptr uft;

        uft = std::make_shared<UseFactoryThread>(objectTestFactory);
        tg.push_back(std::thread(std::bind(&UseFactoryThread::run, uft)));
        objects.push_back(uft);

        uft = std::make_shared<UseFactoryThread>(objectTestFactory, "DerivedObjectTest");
        tg.push_back(std::thread(std::bind(&UseFactoryThread::run, uft)));
        objects.push_back(uft);
    }

    for(size_t i = 0 ; i < NB_THREAD ; i++)
    {
        PopulateRegistryThread::sptr pft;

        pft = std::make_shared<PopulateRegistryThread>(std::ref(objectTestFactory));
        tg.push_back(std::thread(std::bind(&PopulateRegistryThread::run, pft)));
    }

    for(auto& t : tg)
    {
        t.join();
    }

    for(const UseFactoryThreadVector::value_type& uft : objects)
    {
        CPPUNIT_ASSERT_EQUAL(size_t(UseFactoryThread::s_NBOBJECTS), uft->m_objects.size());
    }

    CPPUNIT_ASSERT_EQUAL(NB_THREAD * UseFactoryThread::s_NBOBJECTS* 2, ObjectTest::s_counter);
    CPPUNIT_ASSERT_EQUAL(
        size_t(NB_THREAD * PopulateRegistryThread::s_NBREGISTRYITEMS + 2),
        objectTestFactory.getFactoryKeys().size()
    );
    ObjectTest::s_counter = 0;
}

//-----------------------------------------------------------------------------

} //namespace ut

} //namespace sight::core
