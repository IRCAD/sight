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

#include "SGetVectorTest.hpp"

#include <core/runtime/runtime.hpp>

#include <data/ImageSeries.hpp>
#include <data/ModelSeries.hpp>
#include <data/Vector.hpp>

#include <service/op.hpp>

#include <boost/property_tree/xml_parser.hpp>

#include <sstream>

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION(sight::module::data::ut::SGetVectorTest);

namespace sight::module::data::ut
{

//------------------------------------------------------------------------------

void SGetVectorTest::setUp()
{
    // Set up context before running a test.
    core::runtime::init();
    auto module = core::runtime::load_module(std::string("sight::module::data"));
}

//------------------------------------------------------------------------------

void SGetVectorTest::tearDown()
{
}

//------------------------------------------------------------------------------

void SGetVectorTest::extractsElementFromVector()
{
    auto vector = std::make_shared<sight::data::Vector>();

    sight::service::base::sptr getVector = sight::service::add("sight::module::data::SGetVector");
    CPPUNIT_ASSERT(getVector);

    // create different series
    sight::data::Series::sptr series1 = std::make_shared<sight::data::ModelSeries>();
    sight::data::Series::sptr series2 = std::make_shared<sight::data::ImageSeries>();
    sight::data::Series::sptr series3 = std::make_shared<sight::data::ModelSeries>();
    sight::data::Series::sptr series4 = std::make_shared<sight::data::ImageSeries>();
    sight::data::Series::sptr series5 = std::make_shared<sight::data::ModelSeries>();
    sight::data::Series::sptr series6 = std::make_shared<sight::data::ModelSeries>();

    CPPUNIT_ASSERT(vector->empty());

    vector->push_back(series1);
    vector->push_back(series2);
    vector->push_back(series3);
    vector->push_back(series4);
    vector->push_back(series5);
    vector->push_back(series6);
    CPPUNIT_ASSERT(!vector->empty());
    const std::string index_0ID = "series1";
    const std::string index_3ID = "series4";
    series1->set_id(index_0ID);
    series4->set_id(index_3ID);
    CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(6), vector->size());
    CPPUNIT_ASSERT_EQUAL(index_0ID, series1->get_id());

    service::config_t config;
    std::stringstream config_string;
    config_string
    << "<out group=\"objects\">"
       "<key index=\"0\" uid=\"modelSeries\"/>"
       "<key index=\"3\" uid=\"modelSeries1\"/>"
       "</out>";
    boost::property_tree::read_xml(config_string, config);
    getVector->setConfiguration(config);
    getVector->setInput(vector, "vector");
    getVector->configure();
    getVector->start().wait();
    getVector->update().wait();

    CPPUNIT_ASSERT_EQUAL(getVector->output("objects", 0).lock()->get_id(), index_0ID);
    CPPUNIT_ASSERT_EQUAL(getVector->output("objects", 1).lock()->get_id(), index_3ID);

    vector->clear();

    getVector->update().wait();

    CPPUNIT_ASSERT(getVector->output("objects", 0).lock() == nullptr);
    CPPUNIT_ASSERT(getVector->output("objects", 1).lock() == nullptr);

    getVector->stop().wait();
    sight::service::remove(getVector);
}

//------------------------------------------------------------------------------

void SGetVectorTest::invalidVector()
{
    sight::service::base::sptr getVector = sight::service::add("sight::module::data::SGetVector");

    service::config_t config;
    std::stringstream config_string;
    config_string
    << "<out group=\"vector\">"
       "<key index=\"0\" uid=\"modelSeries\"/>"
       "<key index=\"3\" uid=\"modelSeries1\"/>"
       "</out>";
    boost::property_tree::read_xml(config_string, config);
    getVector->setConfiguration(config);
    getVector->setInput(nullptr, "vector");
    getVector->start().wait();
    CPPUNIT_ASSERT_THROW(getVector->update().get(), sight::data::Exception);

    getVector->stop().wait();
    sight::service::remove(getVector);
}

//------------------------------------------------------------------------------

} // namespace sight::module::data::ut
