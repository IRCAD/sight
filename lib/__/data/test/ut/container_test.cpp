/************************************************************************
 *
 * Copyright (C) 2022-2026 IRCAD France
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

#include <core/compare.hpp>

#include <data/container.hpp>

#include <doctest/doctest.h>

#include <deque>
#include <list>
#include <map>
#include <set>
#include <unordered_map>
#include <unordered_set>
#include <vector>

TEST_SUITE("sight::data::container")
{
//------------------------------------------------------------------------------

    template<class C>
    inline static void test_container(const C& _source, const C& _modified_source)
    {
        class container : public sight::data::container<C>
        {
        public:

            using sight::data::container<C>::container;
            using sight::data::container_wrapper<C>::operator=;
        };

        // Initialization test
        {
            container container(_source);

            // Should be a perfect copy, is_equal will also test default forward iterators
            CHECK(sight::core::is_equal(container, _source));

            class container modified_container(_modified_source);

            // Should be different
            CHECK(!sight::core::is_equal(container, modified_container));
        }

        // Assignment test
        {
            container container;
            container = _source;

            // Should be a perfect copy, is_equal will also test default forward iterators
            CHECK(sight::core::is_equal(container, _source));

            class container modified_container;
            modified_container = _modified_source;

            // Should be different
            CHECK(!sight::core::is_equal(container, modified_container));
        }

        // Initializer list test

        // std::copy test
        {
            container container;

            std::copy(_source.cbegin(), _source.cend(), inserter(container));

            // Should be a perfect copy
            CHECK(sight::core::is_equal(container, _source));

            class container modified_container;

            std::copy(_modified_source.cbegin(), _modified_source.cend(), inserter(modified_container));

            // Should be different (test inequality operator)
            CHECK(!sight::core::is_equal(container, modified_container));
        }

        // Clear test
        if constexpr(sight::core::is_container_dynamic<C>::value)
        {
            container container(_source);
            class container empty;

            CHECK(!sight::core::is_equal(container, empty));

            container.clear();

            CHECK(sight::core::is_equal(container, empty));
        }

        // Special features of vector kind container
        if constexpr(sight::core::is_vector<C>::value)
        {
            container container(_source);

            const auto& element = _source[0];

            // Add some elements
            container.push_back(element);
            container.push_back(element);
            container.push_back(element);

            // Test remove one (container[0] should be removed)
            CHECK(container.size() == (_source.size() + 3));
            container.remove_one(element);
            CHECK(container.size() == (_source.size() + 2));
            CHECK(container.front() != element);
            CHECK(container.back() == element);

            container.remove_one(element);
            CHECK(container.size() == (_source.size() + 1));
            CHECK(container.front() != element);
            CHECK(container.back() == element);

            // Test returned iterator
            const auto& it = container.remove_one(element);
            CHECK(container.size() == _source.size());
            CHECK(container.front() != element);
            CHECK(container.back() == element);
            CHECK_NOTHROW(container.erase(it));
            CHECK(container.size() == (_source.size() - 1));
            CHECK(container.front() != element);
            CHECK(container.back() != element);

            // Removing an element that does not exist should not change the container
            container.remove_one(element);
            CHECK(container.size() == _source.size() - 1);

            // Add some elements
            container.push_back(_source[0]);
            container.push_back(_source[0]);
            container.push_back(_source[0]);

            // Test remove all
            CHECK(container.size() == (_source.size() + 2));
            container.remove(_source[0]);
            CHECK(container.size() == (_source.size() - 1));

            // Removing an element that does not exist should not change the container
            container.remove(_source[0]);
            CHECK(container.size() == (_source.size() - 1));
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
        test_container<sight::data::sequenced_set<C> >(_original_list, _modified_list);
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

    TEST_CASE("array")
    {
    }

//------------------------------------------------------------------------------

    TEST_CASE("generic")
    {
    }

//------------------------------------------------------------------------------

    TEST_CASE("map")
    {
    }
} // TEST_SUITE("sight::data::container")
