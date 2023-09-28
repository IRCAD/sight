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

#include "SPushSelectionTest.hpp"

#include <data/SeriesSet.hpp>
#include <data/Vector.hpp>

#include <service/op.hpp>

CPPUNIT_TEST_SUITE_REGISTRATION(sight::module::ui::series::ut::SPushSelectionTest);

namespace sight::module::ui::series::ut
{

//------------------------------------------------------------------------------

void SPushSelectionTest::setUp()
{
    m_pushSelection = service::add("sight::module::ui::series::SPushSelection");
    CPPUNIT_ASSERT_MESSAGE(
        "Failed to create service 'sight::module::ui::series::SPushSelection'",
        m_pushSelection
    );
}

//------------------------------------------------------------------------------

void SPushSelectionTest::tearDown()
{
    if(!m_pushSelection->isStopped())
    {
        CPPUNIT_ASSERT_NO_THROW(m_pushSelection->stop().get());
    }

    service::remove(m_pushSelection);
}

//------------------------------------------------------------------------------

void SPushSelectionTest::basicTest()
{
    auto selectedSeries = std::make_shared<data::Vector>();
    auto series         = std::make_shared<data::Series>();
    selectedSeries->push_back(series);
    m_pushSelection->setInput(selectedSeries, "selectedSeries");
    auto seriesSet = std::make_shared<data::SeriesSet>();
    m_pushSelection->setInOut(seriesSet, "seriesSet");
    CPPUNIT_ASSERT(seriesSet->empty());
    CPPUNIT_ASSERT_NO_THROW(m_pushSelection->configure());
    CPPUNIT_ASSERT_NO_THROW(m_pushSelection->start().get());
    CPPUNIT_ASSERT_NO_THROW(m_pushSelection->update().get());
    CPPUNIT_ASSERT_NO_THROW(m_pushSelection->stop().get());
    CPPUNIT_ASSERT_EQUAL(std::size_t(1), seriesSet->size());
    CPPUNIT_ASSERT((*seriesSet)[0] == series);
}

//------------------------------------------------------------------------------

} // namespace sight::module::ui::series::ut
