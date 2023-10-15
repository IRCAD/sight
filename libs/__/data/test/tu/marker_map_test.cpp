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

static const data::marker_map::MarkerType marker101 = {{
    {{1.23F, 1.4314F}},
    {{2.45F, 1.42F}},
    {{-1.3F, 1.24F}},
    {{-10.23F, 1.4F}}
}
};

static const data::marker_map::MarkerType marker102 = {{
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
    data::marker_map::sptr markerMap = std::make_shared<data::marker_map>();
    CPPUNIT_ASSERT_EQUAL(std::size_t(0), markerMap->count());

    markerMap->setMarker("101", marker101);
    CPPUNIT_ASSERT_EQUAL(std::size_t(1), markerMap->count());

    markerMap->setMarker("102", marker102);
    CPPUNIT_ASSERT_EQUAL(std::size_t(2), markerMap->count());

    // Not-const getters
    {
        auto* marker = markerMap->getMarker("101");
        CPPUNIT_ASSERT(marker101 == *marker);
        CPPUNIT_ASSERT(marker102 != *marker);

        marker = markerMap->getMarker("102");
        CPPUNIT_ASSERT(marker101 != *marker);
        CPPUNIT_ASSERT(marker102 == *marker);

        {
            const auto& markerRef = markerMap->getMarker(0);
            CPPUNIT_ASSERT(marker101 == markerRef);
            CPPUNIT_ASSERT(marker102 != markerRef);
        }

        {
            const auto& markerRef = markerMap->getMarker(1);
            CPPUNIT_ASSERT(marker101 != markerRef);
            CPPUNIT_ASSERT(marker102 == markerRef);
        }

        marker = markerMap->getMarker("42");
        CPPUNIT_ASSERT(nullptr == marker);
    }

    // Const getters
    {
        data::marker_map::csptr constMarkerMap = markerMap;

        const auto* marker = constMarkerMap->getMarker("101");
        CPPUNIT_ASSERT(marker101 == *marker);
        CPPUNIT_ASSERT(marker102 != *marker);

        marker = constMarkerMap->getMarker("102");
        CPPUNIT_ASSERT(marker101 != *marker);
        CPPUNIT_ASSERT(marker102 == *marker);

        {
            const auto& markerRef = constMarkerMap->getMarker(0);
            CPPUNIT_ASSERT(marker101 == markerRef);
            CPPUNIT_ASSERT(marker102 != markerRef);
        }

        {
            const auto& markerRef = constMarkerMap->getMarker(1);
            CPPUNIT_ASSERT(marker101 != markerRef);
            CPPUNIT_ASSERT(marker102 == markerRef);
        }

        marker = constMarkerMap->getMarker("42");
        CPPUNIT_ASSERT(nullptr == marker);
    }

    CPPUNIT_ASSERT_EQUAL(std::size_t(2), markerMap->count());
}

//------------------------------------------------------------------------------

void marker_map_test::shallow_copyTest()
{
    data::marker_map::sptr markerMap1 = std::make_shared<data::marker_map>();
    markerMap1->setMarker("101", marker101);
    markerMap1->setMarker("102", marker102);

    data::marker_map::sptr markerMap2 = std::make_shared<data::marker_map>();
    CPPUNIT_ASSERT(*markerMap1 != *markerMap2);
    markerMap2->shallow_copy(markerMap1);
    CPPUNIT_ASSERT(*markerMap1 == *markerMap2);

    CPPUNIT_ASSERT(markerMap1->getMarker("101") != markerMap2->getMarker("101"));
    CPPUNIT_ASSERT(*markerMap1->getMarker("101") == *markerMap2->getMarker("101"));
    CPPUNIT_ASSERT(markerMap1->getMarker("0") == markerMap2->getMarker("0"));
}

//------------------------------------------------------------------------------

void marker_map_test::deep_copyTest()
{
    data::marker_map::sptr markerMap1 = std::make_shared<data::marker_map>();
    markerMap1->setMarker("101", marker101);
    markerMap1->setMarker("102", marker102);

    data::marker_map::sptr markerMap2 = std::make_shared<data::marker_map>();
    CPPUNIT_ASSERT(*markerMap1 != *markerMap2);
    markerMap2->deep_copy(markerMap1);
    CPPUNIT_ASSERT(*markerMap1 == *markerMap2);

    CPPUNIT_ASSERT(markerMap1->getMarker("101") != markerMap2->getMarker("101"));
    CPPUNIT_ASSERT(*markerMap1->getMarker("101") == *markerMap2->getMarker("101"));
    CPPUNIT_ASSERT(markerMap1->getMarker("0") == markerMap2->getMarker("0"));
}

} // namespace sight::data::ut
