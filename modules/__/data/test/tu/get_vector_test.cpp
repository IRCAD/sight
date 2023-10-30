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

#include "get_vector_test.hpp"

#include <core/runtime/runtime.hpp>

#include <data/image_series.hpp>
#include <data/model_series.hpp>
#include <data/vector.hpp>

#include <service/op.hpp>

#include <boost/property_tree/xml_parser.hpp>

#include <sstream>

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION(sight::module::data::ut::get_vector_test);

namespace sight::module::data::ut
{

//------------------------------------------------------------------------------

void get_vector_test::setUp()
{
    // Set up context before running a test.
    core::runtime::init();
    auto module = core::runtime::load_module(std::string("sight::module::data"));
}

//------------------------------------------------------------------------------

void get_vector_test::tearDown()
{
}

//------------------------------------------------------------------------------

void get_vector_test::extracts_element_from_vector()
{
    auto vector = std::make_shared<sight::data::vector>();

    sight::service::base::sptr get_vector = sight::service::add("sight::module::data::get_vector");
    CPPUNIT_ASSERT(get_vector);

    // create different series
    sight::data::series::sptr series1 = std::make_shared<sight::data::model_series>();
    sight::data::series::sptr series2 = std::make_shared<sight::data::image_series>();
    sight::data::series::sptr series3 = std::make_shared<sight::data::model_series>();
    sight::data::series::sptr series4 = std::make_shared<sight::data::image_series>();
    sight::data::series::sptr series5 = std::make_shared<sight::data::model_series>();
    sight::data::series::sptr series6 = std::make_shared<sight::data::model_series>();

    CPPUNIT_ASSERT(vector->empty());

    vector->push_back(series1);
    vector->push_back(series2);
    vector->push_back(series3);
    vector->push_back(series4);
    vector->push_back(series5);
    vector->push_back(series6);
    CPPUNIT_ASSERT(!vector->empty());
    const std::string index_0_id = "series1";
    const std::string index_3_id = "series4";
    series1->set_id(index_0_id);
    series4->set_id(index_3_id);
    CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(6), vector->size());
    CPPUNIT_ASSERT_EQUAL(index_0_id, series1->get_id());

    service::config_t config;
    std::stringstream config_string;
    config_string
    << "<out group=\"objects\">"
       "<key index=\"0\" uid=\"modelSeries\"/>"
       "<key index=\"3\" uid=\"modelSeries1\"/>"
       "</out>";
    boost::property_tree::read_xml(config_string, config);
    get_vector->set_config(config);
    get_vector->set_input(vector, "vector");
    get_vector->configure();
    get_vector->start().wait();
    get_vector->update().wait();

    CPPUNIT_ASSERT_EQUAL(get_vector->output("objects", 0).lock()->get_id(), index_0_id);
    CPPUNIT_ASSERT_EQUAL(get_vector->output("objects", 1).lock()->get_id(), index_3_id);

    vector->clear();

    get_vector->update().wait();

    CPPUNIT_ASSERT(get_vector->output("objects", 0).lock() == nullptr);
    CPPUNIT_ASSERT(get_vector->output("objects", 1).lock() == nullptr);

    get_vector->stop().wait();
    sight::service::remove(get_vector);
}

//------------------------------------------------------------------------------

void get_vector_test::invalid_vector()
{
    sight::service::base::sptr get_vector = sight::service::add("sight::module::data::get_vector");

    service::config_t config;
    std::stringstream config_string;
    config_string
    << "<out group=\"vector\">"
       "<key index=\"0\" uid=\"modelSeries\"/>"
       "<key index=\"3\" uid=\"modelSeries1\"/>"
       "</out>";
    boost::property_tree::read_xml(config_string, config);
    get_vector->set_config(config);
    get_vector->set_input(nullptr, "vector");
    get_vector->start().wait();
    CPPUNIT_ASSERT_THROW(get_vector->update().get(), sight::data::exception);

    get_vector->stop().wait();
    sight::service::remove(get_vector);
}

//------------------------------------------------------------------------------

} // namespace sight::module::data::ut
