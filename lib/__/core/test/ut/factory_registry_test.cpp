/************************************************************************
 *
 * Copyright (C) 2009-2025 IRCAD France
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

#include <core/factory_registry.hpp>
#include <core/lazy_instantiator.hpp>
#include <core/mt/types.hpp>
#include <core/spy_log.hpp>

#include <doctest/doctest.h>

#include <algorithm>
#include <chrono>
#include <exception>
#include <functional>
#include <iostream>
#include <thread>

TEST_SUITE("sight::core::factory_registry")
{
//-----------------------------------------------------------------------------

    class object_test
    {
    public:

        using sptr = std::shared_ptr<object_test>;

        object_test() :
            m_name("object_test")
        {
            sight::core::mt::scoped_lock lock(s_mutex);
            ++s_counter;
        }

        explicit object_test(std::string _name) :
            m_name(std::move(_name))
        {
            sight::core::mt::scoped_lock lock(s_mutex);
            ++s_counter;
        }

        explicit object_test(int _msec) :
            m_name("object_test+sleep")
        {
            sight::core::mt::scoped_lock lock(s_mutex);
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
        static sight::core::mt::mutex s_mutex;
    };

    int object_test::s_counter = 0;
    sight::core::mt::mutex object_test::s_mutex;

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

    TEST_CASE("pointer")
    {
        object_test::s_counter = 0;

        using factory_type = sight::core::factory_registry<object_test::sptr()>;
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
        std::ranges::sort(keys);
        factory_type::key_vector_t vect_keys = object_test_factory.get_factory_keys();
        std::ranges::sort(vect_keys);
        CHECK(keys == vect_keys);

        object_test::sptr object_test1 = object_test_factory.create("object_test");
        CHECK_EQ(1, object_test::s_counter);

        object_test::sptr object_test2 = object_test_factory.create("object_test");
        CHECK_EQ(2, object_test::s_counter);

        CHECK_EQ(std::string("object_test"), object_test1->name());
        CHECK_EQ(std::string("object_test"), object_test2->name());

        object_test::sptr derived_object_test1 = object_test_factory.create("DerivedObjectTest");
        CHECK_EQ(3, object_test::s_counter);
        object_test::sptr derived_object_test2 = object_test_factory.create("DerivedObjectTest");
        CHECK_EQ(4, object_test::s_counter);

        CHECK_EQ(std::string("DerivedObjectTest"), derived_object_test1->name());
        CHECK_EQ(std::string("DerivedObjectTest"), derived_object_test2->name());

        object_test::s_counter = 0;
    }

//-----------------------------------------------------------------------------

    TEST_CASE("value")
    {
        object_test::s_counter = 0;

        sight::core::factory_registry<object_test()> object_test_factory;
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
        CHECK_EQ(1, object_test::s_counter);

        object_test object_test2 = object_test_factory.create("object_test");
        CHECK_EQ(2, object_test::s_counter);

        CHECK_EQ(std::string("object_test"), object_test1.name());
        CHECK_EQ(std::string("object_test"), object_test2.name());

        object_test derived_object_test1 = object_test_factory.create("DerivedObjectTest");
        CHECK_EQ(3, object_test::s_counter);
        object_test derived_object_test2 = object_test_factory.create("DerivedObjectTest");
        CHECK_EQ(4, object_test::s_counter);

        CHECK_EQ(std::string("DerivedObjectTest"), derived_object_test1.name());
        CHECK_EQ(std::string("DerivedObjectTest"), derived_object_test2.name());

        object_test::s_counter = 0;
    }

//-----------------------------------------------------------------------------

    TEST_CASE("arg")
    {
        object_test::s_counter = 0;

        sight::core::factory_registry<object_test::sptr(std::string)> object_test_factory;
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
        CHECK_EQ(std::string("ObjectTest1"), object_test1->name());
        CHECK_EQ(std::string("ObjectTest2"), object_test2->name());

        object_test::sptr derived_object_test1;
        object_test::sptr derived_object_test2;
        std::string der_obj_test1("DerivedObjectTest1");
        std::string der_obj_test2("DerivedObjectTest2");
        derived_object_test1 = object_test_factory.create("DerivedObjectTest", der_obj_test1);
        derived_object_test2 = object_test_factory.create("DerivedObjectTest", der_obj_test2);
        CHECK_EQ(std::string("DerivedObjectTest1"), derived_object_test1->name());
        CHECK_EQ(std::string("DerivedObjectTest2"), derived_object_test2->name());

        CHECK_EQ(4, object_test::s_counter);

        object_test::s_counter = 0;
    }

//-----------------------------------------------------------------------------

    using thread_safety_test_factory_type = sight::core::factory_registry<object_test::sptr(int)>;

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

    TEST_CASE("thread_safety")
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
            CHECK_EQ(std::size_t(use_factory_thread::NB_OBJECTS), uft->m_objects.size());
        }

        CHECK_EQ(nb_thread * use_factory_thread::NB_OBJECTS * 2, object_test::s_counter);
        CHECK_EQ(
            std::size_t(nb_thread * populate_registry_thread::NB_REGISTRY_ITEMS + 2),
            object_test_factory.get_factory_keys().size()
        );
        object_test::s_counter = 0;
    }
} // TEST_SUITE
