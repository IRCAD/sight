/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2014-2018.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "MarkerMapTest.hpp"

#include <arData/MarkerMap.hpp>

#include <arDataCamp/Version.hpp>

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION( ::arData::ut::MarkerMapTest );

namespace arData
{
namespace ut
{

static const ::arData::MarkerMap::MarkerType marker101 = {{
                                                              {{ 1.23f, 1.4314f }},
                                                              {{ 2.45f, 1.42f }},
                                                              {{ -1.3f, 1.24f }},
                                                              {{ -10.23f, 1.4f }}
                                                          }};

static const ::arData::MarkerMap::MarkerType marker102 = {{
                                                              {{ 1.23f, 2.4314f }},
                                                              {{ 2.45f, 4.484f }},
                                                              {{ -11.3f, 3.24f }},
                                                              {{ 100.23f, 1.4f }}
                                                          }};

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
    ::arData::MarkerMap::sptr markerMap = ::arData::MarkerMap::New();
    CPPUNIT_ASSERT_EQUAL(size_t(0), markerMap->count());

    markerMap->setMarker("101", marker101);
    CPPUNIT_ASSERT_EQUAL(size_t(1), markerMap->count());

    markerMap->setMarker("102", marker102);
    CPPUNIT_ASSERT_EQUAL(size_t(2), markerMap->count());

    // Not-const getters
    {
        auto marker = markerMap->getMarker("101");
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
        ::arData::MarkerMap::csptr constMarkerMap = markerMap;

        auto marker = constMarkerMap->getMarker("101");
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

    CPPUNIT_ASSERT_EQUAL(size_t(2), markerMap->count());
}

//------------------------------------------------------------------------------

void MarkerMapTest::shallowCopyTest()
{
    ::arData::MarkerMap::sptr markerMap1 = ::arData::MarkerMap::New();
    markerMap1->setMarker("101", marker101);
    markerMap1->setMarker("102", marker102);

    ::arData::MarkerMap::sptr markerMap2 = ::arData::MarkerMap::New();
    markerMap2->shallowCopy(markerMap1);

    CPPUNIT_ASSERT(markerMap1->getMarker("101") != markerMap2->getMarker("101"));
    CPPUNIT_ASSERT(*markerMap1->getMarker("101") == *markerMap2->getMarker("101"));
    CPPUNIT_ASSERT(markerMap1->getMarker("0") == markerMap2->getMarker("0"));
}

//------------------------------------------------------------------------------

void MarkerMapTest::deepCopyTest()
{
    ::arData::MarkerMap::sptr markerMap1 = ::arData::MarkerMap::New();
    markerMap1->setMarker("101", marker101);
    markerMap1->setMarker("102", marker102);

    ::arData::MarkerMap::sptr markerMap2 = ::arData::MarkerMap::New();
    markerMap2->deepCopy(markerMap1);

    CPPUNIT_ASSERT(markerMap1->getMarker("101") != markerMap2->getMarker("101"));
    CPPUNIT_ASSERT(*markerMap1->getMarker("101") == *markerMap2->getMarker("101"));
    CPPUNIT_ASSERT(markerMap1->getMarker("0") == markerMap2->getMarker("0"));
}

} //namespace ut
} //namespace arData
