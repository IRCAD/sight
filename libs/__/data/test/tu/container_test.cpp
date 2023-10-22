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
inline static void test_container(const C& _source, const C& _modified_source)
{
    class Container : public data::container<C>
    {
    public:

        using data::container<C>::container;
        using ContainerWrapper<C>::operator=;
    };

    // Initialization test
    {
        Container container(_source);

        // Should be a perfect copy, is_equal will also test default forward iterators
        CPPUNIT_ASSERT(core::tools::is_equal(container, _source));

        Container modified_container(_modified_source);

        // Should be different
        CPPUNIT_ASSERT(!core::tools::is_equal(container, modified_container));
    }

    // Assignement test
    {
        Container container;
        container = _source;

        // Should be a perfect copy, is_equal will also test default forward iterators
        CPPUNIT_ASSERT(core::tools::is_equal(container, _source));

        Container modified_container;
        modified_container = _modified_source;

        // Should be different
        CPPUNIT_ASSERT(!core::tools::is_equal(container, modified_container));
    }

    // Initializer list test

    // std::copy test
    {
        Container container;

        std::copy(_source.cbegin(), _source.cend(), inserter(container));

        // Should be a perfect copy
        CPPUNIT_ASSERT(core::tools::is_equal(container, _source));

        Container modified_container;

        std::copy(_modified_source.cbegin(), _modified_source.cend(), inserter(modified_container));

        // Should be different (test inequality operator)
        CPPUNIT_ASSERT(!core::tools::is_equal(container, modified_container));
    }

    // Clear test
    if constexpr(core::tools::is_container_dynamic<C>::value)
    {
        Container container(_source);
        Container empty;

        CPPUNIT_ASSERT(!core::tools::is_equal(container, empty));

        container.clear();

        CPPUNIT_ASSERT(core::tools::is_equal(container, empty));
    }

    // Special features of vector kind container
    if constexpr(core::tools::is_vector<C>::value)
    {
        Container container(_source);

        const auto& element = _source[0];

        // Add some elements
        container.push_back(element);
        container.push_back(element);
        container.push_back(element);

        // Test remove one (container[0] should be removed)
        CPPUNIT_ASSERT(container.size() == (_source.size() + 3));
        container.remove_one(element);
        CPPUNIT_ASSERT(container.size() == (_source.size() + 2));
        CPPUNIT_ASSERT(container.front() != element);
        CPPUNIT_ASSERT(container.back() == element);

        container.remove_one(element);
        CPPUNIT_ASSERT(container.size() == (_source.size() + 1));
        CPPUNIT_ASSERT(container.front() != element);
        CPPUNIT_ASSERT(container.back() == element);

        // Test returned iterator
        const auto& it = container.remove_one(element);
        CPPUNIT_ASSERT(container.size() == _source.size());
        CPPUNIT_ASSERT(container.front() != element);
        CPPUNIT_ASSERT(container.back() == element);
        CPPUNIT_ASSERT_NO_THROW(container.erase(it));
        CPPUNIT_ASSERT(container.size() == (_source.size() - 1));
        CPPUNIT_ASSERT(container.front() != element);
        CPPUNIT_ASSERT(container.back() != element);

        // Removing an element that does not exist should not change the container
        container.remove_one(element);
        CPPUNIT_ASSERT(container.size() == _source.size() - 1);

        // Add some elements
        container.push_back(_source[0]);
        container.push_back(_source[0]);
        container.push_back(_source[0]);

        // Test remove all
        CPPUNIT_ASSERT(container.size() == (_source.size() + 2));
        container.remove(_source[0]);
        CPPUNIT_ASSERT(container.size() == (_source.size() - 1));

        // Removing an element that does not exist should not change the container
        container.remove(_source[0]);
        CPPUNIT_ASSERT(container.size() == (_source.size() - 1));
    }
}

//------------------------------------------------------------------------------

template<class C>
inline static void test_dispatcher(
    const std::initializer_list<C>& _original_list,
    const std::initializer_list<C>& _modified_list
)
{
    test_container<std::vector<C> >(_original_list, _modified_list);
    test_container<std::deque<C> >(_original_list, _modified_list);
    test_container<std::list<C> >(_original_list, _modified_list);
    test_container<std::set<C> >(_original_list, _modified_list);
    test_container<std::multiset<C> >(_original_list, _modified_list);
    test_container<std::unordered_set<C> >(_original_list, _modified_list);
    test_container<std::unordered_multiset<C> >(_original_list, _modified_list);
    test_container<sequenced_set<C> >(_original_list, _modified_list);
}

//------------------------------------------------------------------------------

template<class Key, class T>
inline static void map_test_dispatcher(
    const std::initializer_list<std::pair<const Key, T> >& _original_list,
    const std::initializer_list<std::pair<const Key, T> >& _modified_list
)
{
    test_container<std::map<Key, T> >(_original_list, _modified_list);
    test_container<std::multimap<Key, T> >(_original_list, _modified_list);
    test_container<std::unordered_map<Key, T> >(_original_list, _modified_list);
    test_container<std::unordered_multimap<Key, T> >(_original_list, _modified_list);
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
    test_container(
        std::array<int, 3>({1, 2, 3}),
        std::array<int, 3>({4, 5, 6})
    );

    test_container(
        std::array<integer::sptr, 3>(
            {std::make_shared<integer>(1), std::make_shared<integer>(2), std::make_shared<integer>(3)
            }),
        std::array<integer::sptr, 3>(
            {std::make_shared<integer>(4), std::make_shared<integer>(5),
             std::make_shared<integer>(6)
            })
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

void containerTest::generic_test()
{
    test_dispatcher<int>(
        {1, 2, 3},
        {4, 5, 6
        });

    test_dispatcher<integer::sptr>(
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

void containerTest::map_test()
{
    map_test_dispatcher<int, int>({{1, 1}, {2, 2}, {3, 3}}, {{4, 4}, {5, 5}, {6, 6}});

    map_test_dispatcher<int, integer::sptr>(
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
