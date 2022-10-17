/************************************************************************
 *
 * Copyright (C) 2018-2022 IRCAD France
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

#include "MarkerMapTest.hpp"

#include <data/MarkerMap.hpp>

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION(sight::data::ut::MarkerMapTest);

namespace sight::data::ut
{

static const data::MarkerMap::MarkerType marker101 = {{
    {{1.23F, 1.4314F}},
    {{2.45F, 1.42F}},
    {{-1.3F, 1.24F}},
    {{-10.23F, 1.4F}}
}
};

static const data::MarkerMap::MarkerType marker102 = {{
    {{1.23F, 2.4314F}},
    {{2.45F, 4.484F}},
    {{-11.3F, 3.24F}},
    {{100.23F, 1.4F}}
}
};

//------------------------------------------------------------------------------

void MarkerMapTest::setUp()
{
}

//------------------------------------------------------------------------------

void MarkerMapTest::tearDown()
{
}

//------------------------------------------------------------------------------

void MarkerMapTest::getterSetter()
{
    data::MarkerMap::sptr markerMap = data::MarkerMap::New();
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
        data::MarkerMap::csptr constMarkerMap = markerMap;

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

void MarkerMapTest::shallowCopyTest()
{
    data::MarkerMap::sptr markerMap1 = data::MarkerMap::New();
    markerMap1->setMarker("101", marker101);
    markerMap1->setMarker("102", marker102);

    data::MarkerMap::sptr markerMap2 = data::MarkerMap::New();
    CPPUNIT_ASSERT(*markerMap1 != *markerMap2);
    markerMap2->shallowCopy(markerMap1);
    CPPUNIT_ASSERT(*markerMap1 == *markerMap2);

    CPPUNIT_ASSERT(markerMap1->getMarker("101") != markerMap2->getMarker("101"));
    CPPUNIT_ASSERT(*markerMap1->getMarker("101") == *markerMap2->getMarker("101"));
    CPPUNIT_ASSERT(markerMap1->getMarker("0") == markerMap2->getMarker("0"));
}

//------------------------------------------------------------------------------

void MarkerMapTest::deepCopyTest()
{
    data::MarkerMap::sptr markerMap1 = data::MarkerMap::New();
    markerMap1->setMarker("101", marker101);
    markerMap1->setMarker("102", marker102);

    data::MarkerMap::sptr markerMap2 = data::MarkerMap::New();
    CPPUNIT_ASSERT(*markerMap1 != *markerMap2);
    markerMap2->deepCopy(markerMap1);
    CPPUNIT_ASSERT(*markerMap1 == *markerMap2);

    CPPUNIT_ASSERT(markerMap1->getMarker("101") != markerMap2->getMarker("101"));
    CPPUNIT_ASSERT(*markerMap1->getMarker("101") == *markerMap2->getMarker("101"));
    CPPUNIT_ASSERT(markerMap1->getMarker("0") == markerMap2->getMarker("0"));
}

} // namespace sight::data::ut
