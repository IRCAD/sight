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

#include "SeriesSetTest.hpp"

#include <core/tools/UUID.hpp>

#include <data/SeriesSet.hpp>

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION(sight::data::ut::SeriesSetTest);

namespace sight::data::ut
{

//------------------------------------------------------------------------------

void SeriesSetTest::setUp()
{
}

//------------------------------------------------------------------------------

void SeriesSetTest::tearDown()
{
}

//------------------------------------------------------------------------------

void SeriesSetTest::nominalTest()
{
    static const auto description1 = core::tools::UUID::generateUUID();
    static const auto description2 = core::tools::UUID::generateUUID();
    static const auto description3 = core::tools::UUID::generateUUID();

    // Create three series
    const auto& series1 = Series::New();
    series1->setSeriesDescription(description1);

    const auto& series2 = Series::New();
    series2->setSeriesDescription(description2);

    const auto& series3 = Series::New();
    series3->setSeriesDescription(description3);

    // Create an series set and add the three series
    auto series_set = SeriesSet::New();
    CPPUNIT_ASSERT(series_set);

    series_set->push_back(series1);
    series_set->push_back(series2);
    series_set->push_back(series3);

    // Check the size
    CPPUNIT_ASSERT_EQUAL(std::size_t(3), series_set->size());

    // Check that the series set contains the three activities
    // get the set API from the series set, so we can use find()
    const auto& set = series_set->get<1>();
    CPPUNIT_ASSERT(set.find(series1) != set.cend());
    CPPUNIT_ASSERT(set.find(series2) != set.cend());
    CPPUNIT_ASSERT(set.find(series3) != set.cend());

    // SeriesSet is sequenced, like a list
    auto it = series_set->cbegin();
    CPPUNIT_ASSERT(*it == series1);
    CPPUNIT_ASSERT(*(++it) == series2);
    CPPUNIT_ASSERT(*(++it) == series3);

    // It also provide a random access [] operator
    CPPUNIT_ASSERT((*series_set)[0] == series1);
    CPPUNIT_ASSERT((*series_set)[1] == series2);
    CPPUNIT_ASSERT((*series_set)[2] == series3);

    // Like in a std::set, activities stored in an SeriesSet are unique
    series_set->push_back(series1);
    CPPUNIT_ASSERT(series_set->size() == 3);

    // Inserting an already present series should not change the size, and should return false
    CPPUNIT_ASSERT(series_set->insert(series_set->end(), series2).second == false);
    CPPUNIT_ASSERT(series_set->size() == 3);

    // Inserting a new series should change the size, and should return true
    CPPUNIT_ASSERT(series_set->insert(series_set->end(), Series::New()).second == true);
    CPPUNIT_ASSERT(series_set->size() == 4);
}

//------------------------------------------------------------------------------

void SeriesSetTest::copyTest()
{
    static const auto description1 = core::tools::UUID::generateUUID();
    static const auto description2 = core::tools::UUID::generateUUID();
    static const auto description3 = core::tools::UUID::generateUUID();

    // Create three activities
    const auto& series1 = Series::New();
    series1->setSeriesDescription(description1);

    const auto& series2 = Series::New();
    series2->setSeriesDescription(description2);

    const auto& series3 = Series::New();
    series3->setSeriesDescription(description3);

    // Create an series set and add the three activities
    const auto& original_series_set = SeriesSet::New();
    original_series_set->push_back(series1);
    original_series_set->push_back(series2);
    original_series_set->push_back(series3);

    // Create another series and shallow copy the original series
    auto shallow_series_set = SeriesSet::New();
    shallow_series_set->shallowCopy(original_series_set);

    CPPUNIT_ASSERT(shallow_series_set->size() == 3);
    CPPUNIT_ASSERT((*shallow_series_set)[0] == series1);
    CPPUNIT_ASSERT((*shallow_series_set)[1] == series2);
    CPPUNIT_ASSERT((*shallow_series_set)[2] == series3);

    auto deep_series_set = SeriesSet::New();
    deep_series_set->deepCopy(original_series_set);

    CPPUNIT_ASSERT(deep_series_set->size() == 3);

    // Pointers should be different...
    CPPUNIT_ASSERT((*deep_series_set)[0] != series1);
    CPPUNIT_ASSERT((*deep_series_set)[1] != series2);
    CPPUNIT_ASSERT((*deep_series_set)[2] != series3);

    // ...But pointed value not !
    CPPUNIT_ASSERT(is_equal((*deep_series_set)[0], series1));
    CPPUNIT_ASSERT(is_equal((*deep_series_set)[1], series2));
    CPPUNIT_ASSERT(is_equal((*deep_series_set)[2], series3));
}

} // namespace sight::data::ut
