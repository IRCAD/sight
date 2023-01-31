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

#include "SSeriesSignalTest.hpp"

#include <core/service/base.hpp>

#include <data/ImageSeries.hpp>
#include <data/ModelSeries.hpp>
#include <data/Series.hpp>
#include <data/SeriesSet.hpp>

#include <utest/wait.hpp>

CPPUNIT_TEST_SUITE_REGISTRATION(sight::module::data::ut::SSeriesSignalTest);

namespace sight::module::data::ut
{

//------------------------------------------------------------------------------

void SSeriesSignalTest::setUp()
{
    m_seriesSignal = service::add("sight::module::data::SSeriesSignal");
}

//------------------------------------------------------------------------------

void SSeriesSignalTest::tearDown()
{
    m_worker->stop();
    CPPUNIT_ASSERT_NO_THROW(m_seriesSignal->stop().get());
    service::remove(m_seriesSignal);
}

//------------------------------------------------------------------------------

void SSeriesSignalTest::basicTest()
{
    auto seriesSet = sight::data::SeriesSet::New();
    m_seriesSignal->setInput(seriesSet, "seriesSet");
    std::vector<sight::data::Series::sptr> seriesList;
    auto seriesAddedSlot = core::com::newSlot(
        [&](sight::data::Series::sptr series)
        {
            seriesList.push_back(series);
        });
    m_worker = core::thread::Worker::New();
    seriesAddedSlot->setWorker(m_worker);
    m_seriesSignal->signal("seriesAdded")->connect(seriesAddedSlot);
    seriesSet->signal("addedObjects")->connect(m_seriesSignal->slot("reportSeries"));
    CPPUNIT_ASSERT_NO_THROW(m_seriesSignal->configure());
    CPPUNIT_ASSERT_NO_THROW(m_seriesSignal->start().get());

    auto series = sight::data::Series::New();
    {
        auto scopedEmitter = seriesSet->scoped_emit();
        seriesSet->push_back(series);
    }
    /* TODO: fix. With no filter, one would expect that all series are signaled, currently none are.
       fwTestWaitMacro(1 == seriesList.size());
       CPPUNIT_ASSERT_EQUAL(std::size_t(1), seriesList.size());
       CPPUNIT_ASSERT(seriesList[0] == series);*/
}

//------------------------------------------------------------------------------

void SSeriesSignalTest::includeTest()
{
    auto seriesSet = sight::data::SeriesSet::New();
    m_seriesSignal->setInput(seriesSet, "seriesSet");
    std::vector<sight::data::Series::sptr> seriesList;
    auto seriesAddedSlot = core::com::newSlot(
        [&](sight::data::Series::sptr series)
        {
            seriesList.push_back(series);
        });
    m_worker = core::thread::Worker::New();
    seriesAddedSlot->setWorker(m_worker);
    m_seriesSignal->signal("seriesAdded")->connect(seriesAddedSlot);
    seriesSet->signal("addedObjects")->connect(m_seriesSignal->slot("reportSeries"));
    boost::property_tree::ptree ptree;
    ptree.put("filter.mode", "include");
    ptree.put("filter.type", "sight::data::ImageSeries");
    m_seriesSignal->setConfiguration(ptree);
    CPPUNIT_ASSERT_NO_THROW(m_seriesSignal->configure());
    CPPUNIT_ASSERT_NO_THROW(m_seriesSignal->start().get());

    auto imageSeries = sight::data::ImageSeries::New();
    auto modelSeries = sight::data::ModelSeries::New();
    {
        auto scopedEmitter = seriesSet->scoped_emit();
        seriesSet->push_back(imageSeries);
        seriesSet->push_back(modelSeries);
    }
    fwTestWaitMacro(1 == seriesList.size());
    CPPUNIT_ASSERT_EQUAL(std::size_t(1), seriesList.size());
    CPPUNIT_ASSERT(seriesList[0] == imageSeries);
}

//------------------------------------------------------------------------------

void SSeriesSignalTest::excludeTest()
{
    auto seriesSet = sight::data::SeriesSet::New();
    m_seriesSignal->setInput(seriesSet, "seriesSet");
    std::vector<sight::data::Series::sptr> seriesList;
    auto seriesAddedSlot = core::com::newSlot(
        [&](sight::data::Series::sptr series)
        {
            seriesList.push_back(series);
        });
    m_worker = core::thread::Worker::New();
    seriesAddedSlot->setWorker(m_worker);
    m_seriesSignal->signal("seriesAdded")->connect(seriesAddedSlot);
    seriesSet->signal("addedObjects")->connect(m_seriesSignal->slot("reportSeries"));
    boost::property_tree::ptree ptree;
    ptree.put("filter.mode", "exclude");
    ptree.put("filter.type", "sight::data::ImageSeries");
    m_seriesSignal->setConfiguration(ptree);
    CPPUNIT_ASSERT_NO_THROW(m_seriesSignal->configure());
    CPPUNIT_ASSERT_NO_THROW(m_seriesSignal->start().get());

    auto imageSeries = sight::data::ImageSeries::New();
    auto modelSeries = sight::data::ModelSeries::New();
    {
        auto scopedEmitter = seriesSet->scoped_emit();
        seriesSet->push_back(imageSeries);
        seriesSet->push_back(modelSeries);
    }
    /* TODO: fix. exclude filter mode doesn't work as expected.
       fwTestWaitMacro(1 == seriesList.size());
       CPPUNIT_ASSERT_EQUAL(std::size_t(1), seriesList.size());
       CPPUNIT_ASSERT(seriesList[0] == modelSeries);*/
}

} // namespace sight::module::data::ut
