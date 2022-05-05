/************************************************************************
 *
 * Copyright (C) 2022 IRCAD France
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

#include "IContainerTest.hpp"

#include <core/com/Signal.hpp>
#include <core/com/Signal.hxx>
#include <core/tools/compare.hpp>

#include <data/IContainer.hxx>
#include <data/Integer.hpp>

#include <array>
#include <deque>
#include <list>
#include <map>
#include <set>
#include <unordered_map>
#include <unordered_set>
#include <vector>

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION(sight::data::ut::IContainerTest);

namespace sight::data::ut
{

//------------------------------------------------------------------------------

template<class C>
inline static void test_container(const C& source, const C& modified_source)
{
    class Container : public IContainer<C>
    {
    public:

        using IContainer<C>::IContainer;
        using IContainer<C>::operator=;
    };

    // Initialization test
    {
        Container container(source);

        // Should be a perfect copy, is_equal will also test default forward iterators
        CPPUNIT_ASSERT(core::tools::is_equal(container, source));

        Container modified_container(modified_source);

        // Should be different
        CPPUNIT_ASSERT(!core::tools::is_equal(container, modified_container));
    }

    // Assignement test
    {
        Container container;
        container = source;

        // Should be a perfect copy, is_equal will also test default forward iterators
        CPPUNIT_ASSERT(core::tools::is_equal(container, source));

        Container modified_container;
        modified_container = modified_source;

        // Should be different
        CPPUNIT_ASSERT(!core::tools::is_equal(container, modified_container));
    }

    // Initializer list test

    // std::copy test
    {
        Container container;

        std::copy(source.cbegin(), source.cend(), inserter(container));

        // Should be a perfect copy
        CPPUNIT_ASSERT(core::tools::is_equal(container, source));

        Container modified_container;

        std::copy(modified_source.cbegin(), modified_source.cend(), inserter(modified_container));

        // Should be different (test inequality operator)
        CPPUNIT_ASSERT(!core::tools::is_equal(container, modified_container));
    }

    // Clear test
    if constexpr(core::tools::is_container_dynamic<C>::value)
    {
        Container container(source);
        Container empty;

        CPPUNIT_ASSERT(!core::tools::is_equal(container, empty));

        container.clear();

        CPPUNIT_ASSERT(core::tools::is_equal(container, empty));
    }

    // Special features of vector kind container
    if constexpr(core::tools::is_vector<C>::value)
    {
        Container container(source);

        const auto& element = source[0];

        // Add some elements
        container.push_back(element);
        container.push_back(element);
        container.push_back(element);

        // Test remove one (container[0] should be removed)
        CPPUNIT_ASSERT(container.size() == (source.size() + 3));
        container.remove_one(element);
        CPPUNIT_ASSERT(container.size() == (source.size() + 2));
        CPPUNIT_ASSERT(container.front() != element);
        CPPUNIT_ASSERT(container.back() == element);

        container.remove_one(element);
        CPPUNIT_ASSERT(container.size() == (source.size() + 1));
        CPPUNIT_ASSERT(container.front() != element);
        CPPUNIT_ASSERT(container.back() == element);

        // Test returned iterator
        const auto& it = container.remove_one(element);
        CPPUNIT_ASSERT(container.size() == source.size());
        CPPUNIT_ASSERT(container.front() != element);
        CPPUNIT_ASSERT(container.back() == element);
        CPPUNIT_ASSERT_NO_THROW(container.erase(it));
        CPPUNIT_ASSERT(container.size() == (source.size() - 1));
        CPPUNIT_ASSERT(container.front() != element);
        CPPUNIT_ASSERT(container.back() != element);

        // Removing an element that does not exist should not change the container
        container.remove_one(element);
        CPPUNIT_ASSERT(container.size() == source.size() - 1);

        // Add some elements
        container.push_back(source[0]);
        container.push_back(source[0]);
        container.push_back(source[0]);

        // Test remove all
        CPPUNIT_ASSERT(container.size() == (source.size() + 2));
        container.remove_all(source[0]);
        CPPUNIT_ASSERT(container.size() == (source.size() - 1));

        // Removing an element that does not exist should not change the container
        container.remove_all(source[0]);
        CPPUNIT_ASSERT(container.size() == (source.size() - 1));
    }
}

//------------------------------------------------------------------------------

template<class C>
inline static void test_dispatcher(
    const std::initializer_list<C>& original_list,
    const std::initializer_list<C>& modified_list
)
{
    test_container<std::vector<C> >(original_list, modified_list);
    test_container<std::deque<C> >(original_list, modified_list);
    test_container<std::list<C> >(original_list, modified_list);
    test_container<std::set<C> >(original_list, modified_list);
    test_container<std::multiset<C> >(original_list, modified_list);
    test_container<std::unordered_set<C> >(original_list, modified_list);
    test_container<std::unordered_multiset<C> >(original_list, modified_list);
    test_container<sequenced_set<C> >(original_list, modified_list);
}

//------------------------------------------------------------------------------

template<class Key, class T>
inline static void map_test_dispatcher(
    const std::initializer_list<std::pair<const Key, T> >& original_list,
    const std::initializer_list<std::pair<const Key, T> >& modified_list
)
{
    test_container<std::map<Key, T> >(original_list, modified_list);
    test_container<std::multimap<Key, T> >(original_list, modified_list);
    test_container<std::unordered_map<Key, T> >(original_list, modified_list);
    test_container<std::unordered_multimap<Key, T> >(original_list, modified_list);
}

//------------------------------------------------------------------------------

void IContainerTest::setUp()
{
}

//------------------------------------------------------------------------------

void IContainerTest::tearDown()
{
}

//------------------------------------------------------------------------------

void IContainerTest::array_test()
{
    test_container(
        std::array<int, 3>({1, 2, 3}),
        std::array<int, 3>({4, 5, 6})
    );

    test_container(
        std::array<Integer::sptr, 3>({Integer::New(1), Integer::New(2), Integer::New(3)}),
        std::array<Integer::sptr, 3>({Integer::New(4), Integer::New(5), Integer::New(6)})
    );

    test_container(
        std::array<std::shared_ptr<int>, 3>(
        {
            std::make_shared<int>(1),
            std::make_shared<int>(2),
            std::make_shared<int>(3)
        }),
        std::array<std::shared_ptr<int>, 3>(
        {
            std::make_shared<int>(4),
            std::make_shared<int>(5),
            std::make_shared<int>(6)
        })
    );
}

//------------------------------------------------------------------------------

void IContainerTest::generic_test()
{
    test_dispatcher<int>(
        {1, 2, 3},
        {4, 5, 6
        });

    test_dispatcher<Integer::sptr>(
        {
            Integer::New(1),
            Integer::New(2),
            Integer::New(3)
        },
        {
            Integer::New(4),
            Integer::New(5),
            Integer::New(6)
        });

    test_dispatcher<std::shared_ptr<int> >(
        {
            std::make_shared<int>(1),
            std::make_shared<int>(2),
            std::make_shared<int>(3)
        },
        {
            std::make_shared<int>(4),
            std::make_shared<int>(5),
            std::make_shared<int>(6)
        });
}

//------------------------------------------------------------------------------

void IContainerTest::map_test()
{
    map_test_dispatcher<int, int>({{1, 1}, {2, 2}, {3, 3}}, {{4, 4}, {5, 5}, {6, 6}});

    map_test_dispatcher<int, Integer::sptr>(
        {
            {1, Integer::New(1)},
            {2, Integer::New(2)},
            {3, Integer::New(3)}
        },
        {
            {4, Integer::New(4)},
            {5, Integer::New(5)},
            {6, Integer::New(6)}
        });

    map_test_dispatcher<int, std::shared_ptr<int> >(
        {
            {1, std::make_shared<int>(1)},
            {2, std::make_shared<int>(2)},
            {3, std::make_shared<int>(3)}
        },
        {
            {4, std::make_shared<int>(4)},
            {5, std::make_shared<int>(5)},
            {6, std::make_shared<int>(6)}
        });
}

} // namespace sight::data::ut
