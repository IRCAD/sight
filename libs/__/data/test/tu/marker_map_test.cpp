/************************************************************************
 *
 * Copyright (C) 2018-2023 IRCAD France
 * Copyright (C) 2018-2020 IHU Strasbourg
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

#include "marker_map_test.hpp"

#include <data/marker_map.hpp>

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION(sight::data::ut::marker_map_test);

namespace sight::data::ut
{

static const data::marker_map::marker_t marker101 = {{
    {{1.23F, 1.4314F}},
    {{2.45F, 1.42F}},
    {{-1.3F, 1.24F}},
    {{-10.23F, 1.4F}}
}
};

static const data::marker_map::marker_t marker102 = {{
    {{1.23F, 2.4314F}},
    {{2.45F, 4.484F}},
    {{-11.3F, 3.24F}},
    {{100.23F, 1.4F}}
}
};

//------------------------------------------------------------------------------

void marker_map_test::setUp()
{
}

//------------------------------------------------------------------------------

void marker_map_test::tearDown()
{
}

//------------------------------------------------------------------------------

void marker_map_test::getterSetter()
{
    data::marker_map::sptr marker_map = std::make_shared<data::marker_map>();
    CPPUNIT_ASSERT_EQUAL(std::size_t(0), marker_map->count());

    marker_map->setMarker("101", marker101);
    CPPUNIT_ASSERT_EQUAL(std::size_t(1), marker_map->count());

    marker_map->setMarker("102", marker102);
    CPPUNIT_ASSERT_EQUAL(std::size_t(2), marker_map->count());

    // Not-const getters
    {
        auto* marker = marker_map->getMarker("101");
        CPPUNIT_ASSERT(marker101 == *marker);
        CPPUNIT_ASSERT(marker102 != *marker);

        marker = marker_map->getMarker("102");
        CPPUNIT_ASSERT(marker101 != *marker);
        CPPUNIT_ASSERT(marker102 == *marker);

        {
            const auto& marker_ref = marker_map->getMarker(0);
            CPPUNIT_ASSERT(marker101 == marker_ref);
            CPPUNIT_ASSERT(marker102 != marker_ref);
        }

        {
            const auto& marker_ref = marker_map->getMarker(1);
            CPPUNIT_ASSERT(marker101 != marker_ref);
            CPPUNIT_ASSERT(marker102 == marker_ref);
        }

        marker = marker_map->getMarker("42");
        CPPUNIT_ASSERT(nullptr == marker);
    }

    // Const getters
    {
        data::marker_map::csptr const_marker_map = marker_map;

        const auto* marker = const_marker_map->getMarker("101");
        CPPUNIT_ASSERT(marker101 == *marker);
        CPPUNIT_ASSERT(marker102 != *marker);

        marker = const_marker_map->getMarker("102");
        CPPUNIT_ASSERT(marker101 != *marker);
        CPPUNIT_ASSERT(marker102 == *marker);

        {
            const auto& marker_ref = const_marker_map->getMarker(0);
            CPPUNIT_ASSERT(marker101 == marker_ref);
            CPPUNIT_ASSERT(marker102 != marker_ref);
        }

        {
            const auto& marker_ref = const_marker_map->getMarker(1);
            CPPUNIT_ASSERT(marker101 != marker_ref);
            CPPUNIT_ASSERT(marker102 == marker_ref);
        }

        marker = const_marker_map->getMarker("42");
        CPPUNIT_ASSERT(nullptr == marker);
    }

    CPPUNIT_ASSERT_EQUAL(std::size_t(2), marker_map->count());
}

//------------------------------------------------------------------------------

void marker_map_test::shallow_copyTest()
{
    data::marker_map::sptr marker_map1 = std::make_shared<data::marker_map>();
    marker_map1->setMarker("101", marker101);
    marker_map1->setMarker("102", marker102);

    data::marker_map::sptr marker_map2 = std::make_shared<data::marker_map>();
    CPPUNIT_ASSERT(*marker_map1 != *marker_map2);
    marker_map2->shallow_copy(marker_map1);
    CPPUNIT_ASSERT(*marker_map1 == *marker_map2);

    CPPUNIT_ASSERT(marker_map1->getMarker("101") != marker_map2->getMarker("101"));
    CPPUNIT_ASSERT(*marker_map1->getMarker("101") == *marker_map2->getMarker("101"));
    CPPUNIT_ASSERT(marker_map1->getMarker("0") == marker_map2->getMarker("0"));
}

//------------------------------------------------------------------------------

void marker_map_test::deep_copyTest()
{
    data::marker_map::sptr marker_map1 = std::make_shared<data::marker_map>();
    marker_map1->setMarker("101", marker101);
    marker_map1->setMarker("102", marker102);

    data::marker_map::sptr marker_map2 = std::make_shared<data::marker_map>();
    CPPUNIT_ASSERT(*marker_map1 != *marker_map2);
    marker_map2->deep_copy(marker_map1);
    CPPUNIT_ASSERT(*marker_map1 == *marker_map2);

    CPPUNIT_ASSERT(marker_map1->getMarker("101") != marker_map2->getMarker("101"));
    CPPUNIT_ASSERT(*marker_map1->getMarker("101") == *marker_map2->getMarker("101"));
    CPPUNIT_ASSERT(marker_map1->getMarker("0") == marker_map2->getMarker("0"));
}

} // namespace sight::data::ut
