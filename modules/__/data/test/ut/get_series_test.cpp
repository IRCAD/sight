/************************************************************************
 *
 * Copyright (C) 2022-2024 IRCAD France
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

#include "get_series_test.hpp"

#include <core/runtime/runtime.hpp>

#include <data/model_series.hpp>
#include <data/series_set.hpp>

#include <service/op.hpp>

#include <boost/property_tree/xml_parser.hpp>

#include <sstream>

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION(sight::module::data::ut::get_series_test);

namespace sight::module::data::ut
{

//------------------------------------------------------------------------------

void get_series_test::setUp()
{
    // Set up context before running a test.
    core::runtime::init();
    auto module = core::runtime::load_module(std::string("sight::module::data"));
}

//------------------------------------------------------------------------------

void get_series_test::tearDown()
{
}

//------------------------------------------------------------------------------

void get_series_test::extracts_series_from_series_set()
{
    // Set up context before running a test.
    auto series_set = std::make_shared<sight::data::series_set>();

    // Create service
    sight::service::base::sptr get_series = sight::service::add("sight::module::data::get_series");
    CPPUNIT_ASSERT(get_series);
    CPPUNIT_ASSERT(get_series->is_a("sight::module::data::get_series"));

    // create Different models series
    sight::data::series::sptr series1 = std::make_shared<sight::data::model_series>();
    sight::data::series::sptr series2 = std::make_shared<sight::data::model_series>();
    sight::data::series::sptr series3 = std::make_shared<sight::data::model_series>();
    sight::data::series::sptr series4 = std::make_shared<sight::data::model_series>();
    sight::data::series::sptr series5 = std::make_shared<sight::data::model_series>();
    sight::data::series::sptr series6 = std::make_shared<sight::data::model_series>();

    CPPUNIT_ASSERT(series_set->empty());

    series_set->push_back(series1);
    series_set->push_back(series2);
    series_set->push_back(series3);
    series_set->push_back(series4);
    series_set->push_back(series5);
    series_set->push_back(series6);
    CPPUNIT_ASSERT(!series_set->empty());
    const std::string index_3_id = "serie4";
    const std::string index_0_id = "serie1";
    series4->set_id(index_3_id);
    series1->set_id(index_0_id);
    CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(6), series_set->size());
    CPPUNIT_ASSERT_EQUAL(series1, (*series_set)[0]);
    CPPUNIT_ASSERT_EQUAL(series2, (*series_set)[1]);
    CPPUNIT_ASSERT_EQUAL(series3, (*series_set)[2]);
    CPPUNIT_ASSERT_EQUAL(index_0_id, series1->get_id());
    service::config_t config;
    std::stringstream config_string;
    config_string
    << "<out group=\"series\">"
       "<key index=\"0\" uid=\"modelSeries\"/>"
       "<key index=\"3\" uid=\"modelSeries1\"/>"
       "</out>";
    boost::property_tree::read_xml(config_string, config);
    get_series->set_config(config);
    get_series->set_input(series_set, "seriesSet");
    get_series->configure();
    get_series->start().wait();
    get_series->update().wait();

    CPPUNIT_ASSERT_EQUAL(index_0_id, get_series->output("series", 0).lock()->get_id());
    CPPUNIT_ASSERT_EQUAL(index_3_id, get_series->output("series", 1).lock()->get_id());

    get_series->stop().wait();

    sight::service::remove(get_series);
}

//------------------------------------------------------------------------------

void get_series_test::invalid_series_set()
{
    // Create service
    sight::service::base::sptr get_series = sight::service::add("sight::module::data::get_series");
    CPPUNIT_ASSERT(get_series->is_a("sight::module::data::get_series"));

    service::config_t config;
    std::stringstream config_string;
    config_string
    << "<out group=\"series\">"
       "<key index=\"0\" uid=\"modelSeries\"/>"
       "<key index=\"3\" uid=\"modelSeries1\"/>"
       "</out>";
    boost::property_tree::read_xml(config_string, config);
    get_series->set_config(config);
    get_series->set_input(nullptr, "seriesSet");
    get_series->start().wait();
    CPPUNIT_ASSERT_THROW(get_series->update().get(), sight::data::exception);
    get_series->stop().wait();
    sight::service::remove(get_series);
}

//------------------------------------------------------------------------------

} // namespace sight::module::data::ut
