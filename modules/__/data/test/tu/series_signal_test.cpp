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

#include "series_signal_test.hpp"

#include <data/image_series.hpp>
#include <data/model_series.hpp>
#include <data/series.hpp>
#include <data/series_set.hpp>

#include <service/op.hpp>

#include <utest/wait.hpp>

CPPUNIT_TEST_SUITE_REGISTRATION(sight::module::data::ut::series_signal_test);

namespace sight::module::data::ut
{

//------------------------------------------------------------------------------

void series_signal_test::setUp()
{
    m_series_signal = service::add("sight::module::data::series_signal");
}

//------------------------------------------------------------------------------

void series_signal_test::tearDown()
{
    m_worker->stop();
    CPPUNIT_ASSERT_NO_THROW(m_series_signal->stop().get());
    service::remove(m_series_signal);
}

//------------------------------------------------------------------------------

void series_signal_test::basic_test()
{
    auto series_set = std::make_shared<sight::data::series_set>();
    m_series_signal->set_input(series_set, "seriesSet");
    std::vector<sight::data::series::sptr> series_list;
    auto series_added_slot = core::com::new_slot(
        [&](sight::data::series::sptr _series)
        {
            series_list.push_back(_series);
        });
    m_worker = core::thread::worker::make();
    series_added_slot->set_worker(m_worker);
    m_series_signal->signal("seriesAdded")->connect(series_added_slot);
    series_set->signal("addedObjects")->connect(m_series_signal->slot("reportSeries"));
    CPPUNIT_ASSERT_NO_THROW(m_series_signal->configure());
    CPPUNIT_ASSERT_NO_THROW(m_series_signal->start().get());

    auto series = std::make_shared<sight::data::series>();
    {
        auto scoped_emitter = series_set->scoped_emit();
        series_set->push_back(series);
    }
    /* TODO: fix. With no filter, one would expect that all series are signaled, currently none are.
       SIGHT_TEST_WAIT(1 == seriesList.size());
       CPPUNIT_ASSERT_EQUAL(std::size_t(1), seriesList.size());
       CPPUNIT_ASSERT(seriesList[0] == series);*/
}

//------------------------------------------------------------------------------

void series_signal_test::include_test()
{
    auto series_set = std::make_shared<sight::data::series_set>();
    m_series_signal->set_input(series_set, "seriesSet");
    std::vector<sight::data::series::sptr> series_list;
    auto series_added_slot = core::com::new_slot(
        [&](sight::data::series::sptr _series)
        {
            series_list.push_back(_series);
        });
    m_worker = core::thread::worker::make();
    series_added_slot->set_worker(m_worker);
    m_series_signal->signal("seriesAdded")->connect(series_added_slot);
    series_set->signal("addedObjects")->connect(m_series_signal->slot("reportSeries"));
    boost::property_tree::ptree ptree;
    ptree.put("filter.mode", "include");
    ptree.put("filter.type", "sight::data::image_series");
    m_series_signal->set_config(ptree);
    CPPUNIT_ASSERT_NO_THROW(m_series_signal->configure());
    CPPUNIT_ASSERT_NO_THROW(m_series_signal->start().get());

    auto image_series = std::make_shared<sight::data::image_series>();
    auto model_series = std::make_shared<sight::data::model_series>();
    {
        auto scoped_emitter = series_set->scoped_emit();
        series_set->push_back(image_series);
        series_set->push_back(model_series);
    }
    SIGHT_TEST_WAIT(1 == series_list.size());
    CPPUNIT_ASSERT_EQUAL(std::size_t(1), series_list.size());
    CPPUNIT_ASSERT(series_list[0] == image_series);
}

//------------------------------------------------------------------------------

void series_signal_test::exclude_test()
{
    auto series_set = std::make_shared<sight::data::series_set>();
    m_series_signal->set_input(series_set, "seriesSet");
    std::vector<sight::data::series::sptr> series_list;
    auto series_added_slot = core::com::new_slot(
        [&](sight::data::series::sptr _series)
        {
            series_list.push_back(_series);
        });
    m_worker = core::thread::worker::make();
    series_added_slot->set_worker(m_worker);
    m_series_signal->signal("seriesAdded")->connect(series_added_slot);
    series_set->signal("addedObjects")->connect(m_series_signal->slot("reportSeries"));
    boost::property_tree::ptree ptree;
    ptree.put("filter.mode", "exclude");
    ptree.put("filter.type", "sight::data::image_series");
    m_series_signal->set_config(ptree);
    CPPUNIT_ASSERT_NO_THROW(m_series_signal->configure());
    CPPUNIT_ASSERT_NO_THROW(m_series_signal->start().get());

    auto image_series = std::make_shared<sight::data::image_series>();
    auto model_series = std::make_shared<sight::data::model_series>();
    {
        auto scoped_emitter = series_set->scoped_emit();
        series_set->push_back(image_series);
        series_set->push_back(model_series);
    }
    /* TODO: fix. exclude filter mode doesn't work as expected.
       SIGHT_TEST_WAIT(1 == seriesList.size());
       CPPUNIT_ASSERT_EQUAL(std::size_t(1), seriesList.size());
       CPPUNIT_ASSERT(seriesList[0] == modelSeries);*/
}

} // namespace sight::module::data::ut
