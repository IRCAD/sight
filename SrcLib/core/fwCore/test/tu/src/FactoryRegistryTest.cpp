/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2012.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <iostream>
#include <exception>

#include <boost/assign/list_of.hpp>
#include <boost/foreach.hpp>
#include <boost/thread.hpp>

#include <fwCore/util/LazyInstantiator.hpp>
#include <fwCore/util/FactoryRegistry.hpp>
#include <fwCore/mt/types.hpp>
#include <fwCore/spyLog.hpp>

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

    ObjectTest(std::string name) : m_name(name)
    {
        ::fwCore::mt::ScopedLock lock(s_mutex);
        ++s_counter;
    }

    ObjectTest(int msec) : m_name("ObjectTest+sleep")
    {
        ::fwCore::mt::ScopedLock lock(s_mutex);
        ::boost::this_thread::sleep(::boost::posix_time::milliseconds(msec));
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

    DerivedObjectTest(std::string name)  : ObjectTest(name)
    {
    }

    DerivedObjectTest(int msec)  : ObjectTest(msec)
    {
    }
};

//-----------------------------------------------------------------------------

void FactoryRegistryTest::pointerTest()
{
    ObjectTest::s_counter = 0;

    typedef ::fwCore::util::FactoryRegistry< ObjectTest::sptr() > FactoryType;
    FactoryType objectTestFactory;
    objectTestFactory.addFactory("ObjectTest", ::boost::factory<ObjectTest::sptr>());
    objectTestFactory.addFactory("DerivedObjectTest", ::boost::factory<DerivedObjectTest::sptr>());

    FactoryType::KeyVectorType keys = ::boost::assign::list_of("ObjectTest") ("DerivedObjectTest");
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

//-----------------------------------------------------------------------------

void FactoryRegistryTest::argTest()
{
    ObjectTest::s_counter = 0;

    ::fwCore::util::FactoryRegistry< ObjectTest::sptr(std::string) > objectTestFactory;
    objectTestFactory.addFactory("ObjectTest", ::boost::factory<ObjectTest::sptr>());
    objectTestFactory.addFactory("DerivedObjectTest", ::boost::factory<DerivedObjectTest::sptr>());

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

typedef ::fwCore::util::FactoryRegistry< ObjectTest::sptr(int) > ThreadSafetyTestFactoryType;

struct UseFactoryThread
{

    typedef ::boost::shared_ptr< UseFactoryThread > sptr;
    typedef std::vector< ObjectTest::sptr > ObjectVectorType;

    UseFactoryThread( const ThreadSafetyTestFactoryType &factory, std::string objType = "ObjectTest" ) :
        m_factory(factory), m_objectType(objType)
    {}

    void run()
    {
        int duration = 20;
        for (int i=0; i< s_NBOBJECTS; ++i)
        {
            OSLM_WARN( "building 1 " << m_objectType << "... " );
            m_objects.push_back( m_factory.create(m_objectType, duration) );
            OSLM_WARN( "built 1 " << m_objectType << "... " );
        }
    }

    const ThreadSafetyTestFactoryType &m_factory;
    ObjectVectorType m_objects;
    std::string m_objectType;
    static const int s_NBOBJECTS;
};

const int UseFactoryThread::s_NBOBJECTS = 10;


struct PopulateRegistryThread
{
    typedef ::boost::shared_ptr< PopulateRegistryThread > sptr;
    typedef std::vector< ObjectTest::sptr > ObjectVectorType;

    PopulateRegistryThread( ThreadSafetyTestFactoryType &factory ) :
        m_factory(factory)
    {}

    void run()
    {
        for (int i=0; i< s_NBREGISTRYITEMS; ++i)
        {
            std::stringstream ss;
            ss << "PopulateFactoryThreadObject-" << ::boost::this_thread::get_id() <<"-" << i ;
            std::string name = ss.str();
            OSLM_WARN( "adding " << name << "... " );
            m_factory.addFactory(name, ::boost::factory<ObjectTest::sptr>());
            OSLM_WARN( "added " << name << "... " );
            ::boost::this_thread::sleep(::boost::posix_time::milliseconds(1));
        }

    }

    ThreadSafetyTestFactoryType &m_factory;
    static const int s_NBREGISTRYITEMS;
};

const int PopulateRegistryThread::s_NBREGISTRYITEMS = 1000;


void FactoryRegistryTest::threadSafetyTest()
{
    ObjectTest::s_counter = 0;

    ThreadSafetyTestFactoryType objectTestFactory;
    objectTestFactory.addFactory("ObjectTest", ::boost::factory<ObjectTest::sptr>());
    objectTestFactory.addFactory("DerivedObjectTest", ::boost::factory<DerivedObjectTest::sptr>());

    const int NB_THREAD(10);

    typedef std::vector < UseFactoryThread::sptr > UseFactoryThreadVector;
    ::boost::thread_group tg;

    UseFactoryThreadVector objects;
    for(size_t i = 0; i < NB_THREAD; i++)
    {
        UseFactoryThread::sptr uft;
        ::boost::thread* t;

        uft = ::boost::make_shared<UseFactoryThread>(objectTestFactory);
        t = new ::boost::thread(::boost::bind(&UseFactoryThread::run, uft) );
        tg.add_thread(t);
        objects.push_back(uft);

        uft = ::boost::make_shared<UseFactoryThread>(objectTestFactory, "DerivedObjectTest");
        t = new ::boost::thread(::boost::bind(&UseFactoryThread::run, uft) );
        tg.add_thread(t);
        objects.push_back(uft);
    }

    for(size_t i = 0; i < NB_THREAD; i++)
    {
        PopulateRegistryThread::sptr pft;
        ::boost::thread* t;

        pft = ::boost::make_shared<PopulateRegistryThread>(::boost::ref(objectTestFactory));
        t = new ::boost::thread(::boost::bind(&PopulateRegistryThread::run, pft) );
        tg.add_thread(t);
    }


    tg.join_all();

    BOOST_FOREACH(UseFactoryThreadVector::value_type uft, objects)
    {
        CPPUNIT_ASSERT_EQUAL(size_t(UseFactoryThread::s_NBOBJECTS), uft->m_objects.size());
    }

    CPPUNIT_ASSERT_EQUAL(NB_THREAD * UseFactoryThread::s_NBOBJECTS * 2, ObjectTest::s_counter);
    CPPUNIT_ASSERT_EQUAL(
                         size_t(NB_THREAD * PopulateRegistryThread::s_NBREGISTRYITEMS + 2),
                         objectTestFactory.getFactoryKeys().size()
                         );
    ObjectTest::s_counter = 0;

}

//-----------------------------------------------------------------------------

} //namespace ut
} //namespace fwCore
