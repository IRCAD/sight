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

#include "SGetSeriesTest.hpp"

#include <data/ModelSeries.hpp>
#include <data/SeriesSet.hpp>

#include <service/base.hpp>

#include <boost/property_tree/xml_parser.hpp>

#include <sstream>

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION(sight::module::data::ut::SGetSeriesTest);

namespace sight::module::data::ut
{

//------------------------------------------------------------------------------

void SGetSeriesTest::setUp()
{
    // Set up context before running a test.
    core::runtime::init();
    auto module = core::runtime::loadModule(std::string("sight::module::data"));
}

//------------------------------------------------------------------------------

void SGetSeriesTest::tearDown()
{
}

//------------------------------------------------------------------------------

void SGetSeriesTest::extractsSeriesFromSeriesSet()
{
    // Set up context before running a test.
    auto series_set = sight::data::SeriesSet::New();

    // Create service
    sight::service::IService::sptr getSeries = sight::service::add("sight::module::data::SGetSeries");
    CPPUNIT_ASSERT(getSeries);

    // create Different models series
    sight::data::Series::sptr series1 = sight::data::ModelSeries::New();
    sight::data::Series::sptr series2 = sight::data::ModelSeries::New();
    sight::data::Series::sptr series3 = sight::data::ModelSeries::New();
    sight::data::Series::sptr series4 = sight::data::ModelSeries::New();
    sight::data::Series::sptr series5 = sight::data::ModelSeries::New();
    sight::data::Series::sptr series6 = sight::data::ModelSeries::New();

    CPPUNIT_ASSERT(series_set->empty());

    series_set->push_back(series1);
    series_set->push_back(series2);
    series_set->push_back(series3);
    series_set->push_back(series4);
    series_set->push_back(series5);
    series_set->push_back(series6);
    CPPUNIT_ASSERT(!series_set->empty());
    const std::string index_3ID = "serie4";
    const std::string index_0ID = "serie1";
    series4->setID(index_3ID);
    series1->setID(index_0ID);
    CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(6), series_set->size());
    CPPUNIT_ASSERT_EQUAL(series1, (*series_set)[0]);
    CPPUNIT_ASSERT_EQUAL(series2, (*series_set)[1]);
    CPPUNIT_ASSERT_EQUAL(series3, (*series_set)[2]);
    CPPUNIT_ASSERT_EQUAL(index_0ID, series1->getID());
    service::IService::ConfigType config;
    std::stringstream config_string;
    config_string
    << "<out group=\"series\">"
       "<key index=\"0\" uid=\"modelSeries\"/>"
       "<key index=\"3\" uid=\"modelSeries1\"/>"
       "</out>";
    boost::property_tree::read_xml(config_string, config);
    getSeries->setConfiguration(config);
    getSeries->setInput(series_set, "seriesSet");
    getSeries->configure();
    getSeries->start().wait();
    getSeries->update().wait();
    auto outputSeries = getSeries->getOutputs();
    SIGHT_ASSERT("index should have size two", outputSeries.size() == 2);
    std::vector<std::string> seriesID;
    seriesID.reserve(2);
    for(auto& entry : outputSeries)
    {
        auto sObject = entry.second.lock();
        auto ID      = sObject.get_shared();
        seriesID.push_back(ID->getID());
    }

    CPPUNIT_ASSERT_EQUAL(index_0ID, seriesID[0]);
    CPPUNIT_ASSERT_EQUAL(index_3ID, seriesID[1]);

    getSeries->stop().wait();

    sight::service::remove(getSeries);
}

//------------------------------------------------------------------------------

void SGetSeriesTest::invalidSeriesSet()
{
    // Create service
    sight::service::IService::sptr getSeries = sight::service::add("sight::module::data::SGetSeries");

    service::IService::ConfigType config;
    std::stringstream config_string;
    config_string
    << "<out group=\"series\">"
       "<key index=\"0\" uid=\"modelSeries\"/>"
       "<key index=\"3\" uid=\"modelSeries1\"/>"
       "</out>";
    boost::property_tree::read_xml(config_string, config);
    getSeries->setConfiguration(config);
    getSeries->setInput(nullptr, "seriesSet");
    getSeries->start().wait();
    CPPUNIT_ASSERT_THROW(getSeries->update().get(), sight::data::Exception);
    getSeries->stop().wait();
    sight::service::remove(getSeries);
}

//------------------------------------------------------------------------------

} // namespace sight::module::data::ut
