/************************************************************************
 *
 * Copyright (C) 2009-2024 IRCAD France
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

#include "factory_registry_test.hpp"

#include <core/factory_registry.hpp>
#include <core/lazy_instantiator.hpp>
#include <core/mt/types.hpp>
#include <core/spy_log.hpp>

#include <chrono>
#include <exception>
#include <functional>
#include <iostream>
#include <thread>

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION(sight::core::ut::factory_registry_test);

namespace sight::core::ut
{

//------------------------------------------------------------------------------

void factory_registry_test::setUp()
{
    // Set up context before running a test.
}

//------------------------------------------------------------------------------

void factory_registry_test::tearDown()
{
    // Clean up after the test run.
}

//-----------------------------------------------------------------------------

class object_test
{
public:

    using sptr = std::shared_ptr<object_test>;

    object_test() :
        m_name("object_test")
    {
        core::mt::scoped_lock lock(s_mutex);
        ++s_counter;
    }

    explicit object_test(std::string _name) :
        m_name(std::move(_name))
    {
        core::mt::scoped_lock lock(s_mutex);
        ++s_counter;
    }

    explicit object_test(int _msec) :
        m_name("object_test+sleep")
    {
        core::mt::scoped_lock lock(s_mutex);
        std::this_thread::sleep_for(std::chrono::milliseconds(_msec));
        ++s_counter;
    }

    virtual ~object_test()
    = default;

    //------------------------------------------------------------------------------

    [[nodiscard]] virtual std::string name() const
    {
        return m_name;
    }

    std::string m_name;
    static int s_counter;
    static core::mt::mutex s_mutex;
};

int object_test::s_counter = 0;
core::mt::mutex object_test::s_mutex;

class derived_object_test : public object_test
{
public:

    using sptr = std::shared_ptr<derived_object_test>;

    derived_object_test()
    {
        m_name = "DerivedObjectTest";
    }

    explicit derived_object_test(const std::string& _name) :
        object_test(_name)
    {
    }

    explicit derived_object_test(int _msec) :
        object_test(_msec)
    {
    }
};

//-----------------------------------------------------------------------------

void factory_registry_test::pointer_test()
{
    object_test::s_counter = 0;

    using factory_type = core::factory_registry<object_test::sptr()>;
    factory_type object_test_factory;
    object_test_factory.add_factory(
        "object_test",
        []() -> object_test::sptr
        {
            return std::make_shared<object_test>();
        });
    object_test_factory.add_factory(
        "DerivedObjectTest",
        []() -> derived_object_test::sptr
        {
            return std::make_shared<derived_object_test>();
        });

    factory_type::key_vector_t keys = {"object_test", "DerivedObjectTest"};
    std::sort(keys.begin(), keys.end());
    factory_type::key_vector_t vect_keys = object_test_factory.get_factory_keys();
    std::sort(vect_keys.begin(), vect_keys.end());
    CPPUNIT_ASSERT(keys == vect_keys);

    object_test::sptr object_test1 = object_test_factory.create("object_test");
    CPPUNIT_ASSERT_EQUAL(1, object_test::s_counter);

    object_test::sptr object_test2 = object_test_factory.create("object_test");
    CPPUNIT_ASSERT_EQUAL(2, object_test::s_counter);

    CPPUNIT_ASSERT_EQUAL(std::string("object_test"), object_test1->name());
    CPPUNIT_ASSERT_EQUAL(std::string("object_test"), object_test2->name());

    object_test::sptr derived_object_test1 = object_test_factory.create("DerivedObjectTest");
    CPPUNIT_ASSERT_EQUAL(3, object_test::s_counter);
    object_test::sptr derived_object_test2 = object_test_factory.create("DerivedObjectTest");
    CPPUNIT_ASSERT_EQUAL(4, object_test::s_counter);

    CPPUNIT_ASSERT_EQUAL(std::string("DerivedObjectTest"), derived_object_test1->name());
    CPPUNIT_ASSERT_EQUAL(std::string("DerivedObjectTest"), derived_object_test2->name());

    object_test::s_counter = 0;
}

//-----------------------------------------------------------------------------

void factory_registry_test::value_test()
{
    object_test::s_counter = 0;

    core::factory_registry<object_test()> object_test_factory;
    object_test_factory.add_factory(
        "object_test",
        []() -> object_test
        {
            return {};
        });
    object_test_factory.add_factory(
        "DerivedObjectTest",
        []() -> derived_object_test
        {
            return {};
        });

    object_test object_test1 = object_test_factory.create("object_test");
    CPPUNIT_ASSERT_EQUAL(1, object_test::s_counter);

    object_test object_test2 = object_test_factory.create("object_test");
    CPPUNIT_ASSERT_EQUAL(2, object_test::s_counter);

    CPPUNIT_ASSERT_EQUAL(std::string("object_test"), object_test1.name());
    CPPUNIT_ASSERT_EQUAL(std::string("object_test"), object_test2.name());

    object_test derived_object_test1 = object_test_factory.create("DerivedObjectTest");
    CPPUNIT_ASSERT_EQUAL(3, object_test::s_counter);
    object_test derived_object_test2 = object_test_factory.create("DerivedObjectTest");
    CPPUNIT_ASSERT_EQUAL(4, object_test::s_counter);

    CPPUNIT_ASSERT_EQUAL(std::string("DerivedObjectTest"), derived_object_test1.name());
    CPPUNIT_ASSERT_EQUAL(std::string("DerivedObjectTest"), derived_object_test2.name());

    object_test::s_counter = 0;
}

//-----------------------------------------------------------------------------

void factory_registry_test::arg_test()
{
    object_test::s_counter = 0;

    core::factory_registry<object_test::sptr(std::string)> object_test_factory;
    object_test_factory.add_factory(
        "object_test",
        [](const std::string& _name) -> object_test::sptr
        {
            return std::make_shared<object_test>(_name);
        });
    object_test_factory.add_factory(
        "DerivedObjectTest",
        [](const std::string& _name) -> derived_object_test::sptr
        {
            return std::make_shared<derived_object_test>(_name);
        });

    std::string obj_test1("ObjectTest1");
    std::string obj_test2("ObjectTest2");
    object_test::sptr object_test1 = object_test_factory.create("object_test", obj_test1);
    object_test::sptr object_test2 = object_test_factory.create("object_test", obj_test2);
    CPPUNIT_ASSERT_EQUAL(std::string("ObjectTest1"), object_test1->name());
    CPPUNIT_ASSERT_EQUAL(std::string("ObjectTest2"), object_test2->name());

    object_test::sptr derived_object_test1;
    object_test::sptr derived_object_test2;
    std::string der_obj_test1("DerivedObjectTest1");
    std::string der_obj_test2("DerivedObjectTest2");
    derived_object_test1 = object_test_factory.create("DerivedObjectTest", der_obj_test1);
    derived_object_test2 = object_test_factory.create("DerivedObjectTest", der_obj_test2);
    CPPUNIT_ASSERT_EQUAL(std::string("DerivedObjectTest1"), derived_object_test1->name());
    CPPUNIT_ASSERT_EQUAL(std::string("DerivedObjectTest2"), derived_object_test2->name());

    CPPUNIT_ASSERT_EQUAL(4, object_test::s_counter);

    object_test::s_counter = 0;
}

//-----------------------------------------------------------------------------

using thread_safety_test_factory_type = core::factory_registry<object_test::sptr(int)>;

struct use_factory_thread
{
    using sptr               = std::shared_ptr<use_factory_thread>;
    using object_vector_type = std::vector<object_test::sptr>;

    explicit use_factory_thread(
        const thread_safety_test_factory_type& _factory,
        std::string _obj_type = "object_test"
    ) :
        m_factory(_factory),
        m_object_type(std::move(_obj_type))
    {
    }

    //------------------------------------------------------------------------------

    void run()
    {
        int duration = 20;
        for(int i = 0 ; i < NB_OBJECTS ; ++i)
        {
            SIGHT_WARN("building 1 " << m_object_type << "... ");
            m_objects.push_back(m_factory.create(m_object_type, duration));
            SIGHT_WARN("built 1 " << m_object_type << "... ");
        }
    }

    const thread_safety_test_factory_type& m_factory;
    object_vector_type m_objects;
    std::string m_object_type;
    static const int NB_OBJECTS;
};

const int use_factory_thread::NB_OBJECTS = 10;

struct populate_registry_thread
{
    using sptr               = std::shared_ptr<populate_registry_thread>;
    using object_vector_type = std::vector<object_test::sptr>;

    explicit populate_registry_thread(thread_safety_test_factory_type& _factory) :
        m_factory(_factory)
    {
    }

    //------------------------------------------------------------------------------

    void run()
    {
        for(int i = 0 ; i < NB_REGISTRY_ITEMS ; ++i)
        {
            std::stringstream ss;
            ss << "PopulateFactoryThreadObject-" << std::this_thread::get_id() << "-" << i;
            std::string name = ss.str();
            SIGHT_WARN("adding " + name + "... ");
            m_factory.add_factory(
                name,
                [](int _msec) -> object_test::sptr
                {
                    return std::make_shared<object_test>(_msec);
                });
            SIGHT_WARN("added " + name + "... ");
            std::this_thread::sleep_for(std::chrono::milliseconds(1));
        }
    }

    thread_safety_test_factory_type& m_factory;
    static const int NB_REGISTRY_ITEMS;
};

const int populate_registry_thread::NB_REGISTRY_ITEMS = 1000;

//------------------------------------------------------------------------------

void factory_registry_test::thread_safety_test()
{
    object_test::s_counter = 0;

    thread_safety_test_factory_type object_test_factory;
    object_test_factory.add_factory(
        "object_test",
        [](int _msec) -> object_test::sptr
        {
            return std::make_shared<object_test>(_msec);
        });
    object_test_factory.add_factory(
        "DerivedObjectTest",
        [](int _msec) -> derived_object_test::sptr
        {
            return std::make_shared<derived_object_test>(_msec);
        });

    const int nb_thread(10);

    using use_factory_thread_vector = std::vector<use_factory_thread::sptr>;
    std::vector<std::thread> tg;

    use_factory_thread_vector objects;
    for(std::size_t i = 0 ; i < nb_thread ; i++)
    {
        use_factory_thread::sptr uft;

        uft = std::make_shared<use_factory_thread>(object_test_factory);
        tg.emplace_back([uft](auto&& ...){uft->run();});
        objects.push_back(uft);

        uft = std::make_shared<use_factory_thread>(object_test_factory, "DerivedObjectTest");
        tg.emplace_back([uft](auto&& ...){uft->run();});
        objects.push_back(uft);
    }

    for(std::size_t i = 0 ; i < nb_thread ; i++)
    {
        populate_registry_thread::sptr pft;

        pft = std::make_shared<populate_registry_thread>(std::ref(object_test_factory));
        tg.emplace_back([pft](auto&& ...){pft->run();});
    }

    for(auto& t : tg)
    {
        t.join();
    }

    for(const auto& uft : objects)
    {
        CPPUNIT_ASSERT_EQUAL(std::size_t(use_factory_thread::NB_OBJECTS), uft->m_objects.size());
    }

    CPPUNIT_ASSERT_EQUAL(nb_thread * use_factory_thread::NB_OBJECTS * 2, object_test::s_counter);
    CPPUNIT_ASSERT_EQUAL(
        std::size_t(nb_thread * populate_registry_thread::NB_REGISTRY_ITEMS + 2),
        object_test_factory.get_factory_keys().size()
    );
    object_test::s_counter = 0;
}

//-----------------------------------------------------------------------------

} // namespace sight::core::ut
