/************************************************************************
 *
 * Copyright (C) 2022-2023 IRCAD France
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

#include "container_test.hpp"

#include <core/com/signal.hpp>
#include <core/com/signal.hxx>
#include <core/tools/compare.hpp>

#include <data/container.hxx>
#include <data/integer.hpp>

#include <array>
#include <deque>
#include <list>
#include <map>
#include <set>
#include <unordered_map>
#include <unordered_set>
#include <vector>

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION(sight::data::ut::containerTest);

namespace sight::data::ut
{

//------------------------------------------------------------------------------

template<class C>
inline static void testContainer(const C& source, const C& modified_source)
{
    class Container : public data::container<C>
    {
    public:

        using data::container<C>::container;
        using ContainerWrapper<C>::operator=;
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
        container.remove(source[0]);
        CPPUNIT_ASSERT(container.size() == (source.size() - 1));

        // Removing an element that does not exist should not change the container
        container.remove(source[0]);
        CPPUNIT_ASSERT(container.size() == (source.size() - 1));
    }
}

//------------------------------------------------------------------------------

template<class C>
inline static void testDispatcher(
    const std::initializer_list<C>& original_list,
    const std::initializer_list<C>& modified_list
)
{
    testContainer<std::vector<C> >(original_list, modified_list);
    testContainer<std::deque<C> >(original_list, modified_list);
    testContainer<std::list<C> >(original_list, modified_list);
    testContainer<std::set<C> >(original_list, modified_list);
    testContainer<std::multiset<C> >(original_list, modified_list);
    testContainer<std::unordered_set<C> >(original_list, modified_list);
    testContainer<std::unordered_multiset<C> >(original_list, modified_list);
    testContainer<sequenced_set<C> >(original_list, modified_list);
}

//------------------------------------------------------------------------------

template<class Key, class T>
inline static void mapTestDispatcher(
    const std::initializer_list<std::pair<const Key, T> >& original_list,
    const std::initializer_list<std::pair<const Key, T> >& modified_list
)
{
    testContainer<std::map<Key, T> >(original_list, modified_list);
    testContainer<std::multimap<Key, T> >(original_list, modified_list);
    testContainer<std::unordered_map<Key, T> >(original_list, modified_list);
    testContainer<std::unordered_multimap<Key, T> >(original_list, modified_list);
}

//------------------------------------------------------------------------------

void containerTest::setUp()
{
}

//------------------------------------------------------------------------------

void containerTest::tearDown()
{
}

//------------------------------------------------------------------------------

void containerTest::array_test()
{
    testContainer(
        std::array<int, 3>({1, 2, 3}),
        std::array<int, 3>({4, 5, 6})
    );

    testContainer(
        std::array<integer::sptr, 3>(
            {std::make_shared<integer>(1), std::make_shared<integer>(2), std::make_shared<integer>(3)
            }),
        std::array<integer::sptr, 3>(
            {std::make_shared<integer>(4), std::make_shared<integer>(5),
             std::make_shared<integer>(6)
            })
    );

    testContainer(
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

void containerTest::generic_test()
{
    testDispatcher<int>(
        {1, 2, 3},
        {4, 5, 6
        });

    testDispatcher<integer::sptr>(
        {
            std::make_shared<integer>(1),
            std::make_shared<integer>(2),
            std::make_shared<integer>(3)
        },
        {
            std::make_shared<integer>(4),
            std::make_shared<integer>(5),
            std::make_shared<integer>(6)
        });

    testDispatcher<std::shared_ptr<int> >(
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

void containerTest::map_test()
{
    mapTestDispatcher<int, int>({{1, 1}, {2, 2}, {3, 3}}, {{4, 4}, {5, 5}, {6, 6}});

    mapTestDispatcher<int, integer::sptr>(
        {
            {1, std::make_shared<integer>(1)},
            {2, std::make_shared<integer>(2)},
            {3, std::make_shared<integer>(3)}
        },
        {
            {4, std::make_shared<integer>(4)},
            {5, std::make_shared<integer>(5)},
            {6, std::make_shared<integer>(6)}
        });

    mapTestDispatcher<int, std::shared_ptr<int> >(
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
