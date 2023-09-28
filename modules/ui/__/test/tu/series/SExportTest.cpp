/************************************************************************
 *
 * Copyright (C) 2023 IRCAD France
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

#include "SExportTest.hpp"

#include <data/Series.hpp>
#include <data/SeriesSet.hpp>

#include <service/op.hpp>

#include <ui/__/dialog/input_dummy.hpp>
#include <ui/__/macros.hpp>

#include <utest/wait.hpp>

CPPUNIT_TEST_SUITE_REGISTRATION(sight::module::ui::series::ut::SExportTest);

SIGHT_REGISTER_GUI(sight::ui::dialog::input_dummy, sight::ui::dialog::input_base::REGISTRY_KEY);

namespace sight::module::ui::series::ut
{

//------------------------------------------------------------------------------

void SExportTest::setUp()
{
    m_export = service::add("sight::module::ui::series::SExport");
    CPPUNIT_ASSERT_MESSAGE("Failed to create service 'sight::module::ui::series::SExport'", m_export);
}

//------------------------------------------------------------------------------

void SExportTest::tearDown()
{
    if(!m_export->isStopped())
    {
        CPPUNIT_ASSERT_NO_THROW(m_export->stop().get());
    }

    service::remove(m_export);
}

//------------------------------------------------------------------------------

void SExportTest::basicTest()
{
    auto data = std::make_shared<data::Series>();
    m_export->setInOut(data, "data");
    auto container = std::make_shared<data::SeriesSet>();
    m_export->setInOut(container, "container");
    CPPUNIT_ASSERT(container->empty());
    CPPUNIT_ASSERT_NO_THROW(m_export->configure());
    CPPUNIT_ASSERT_NO_THROW(m_export->start().get());
    sight::ui::dialog::input_dummy::pushInput("I don't care");
    CPPUNIT_ASSERT_NO_THROW(m_export->update().get());
    CPPUNIT_ASSERT_NO_THROW(m_export->stop().get());
    CPPUNIT_ASSERT_EQUAL(std::size_t(1), container->size());
    CPPUNIT_ASSERT((*container)[0] == data);
}

//------------------------------------------------------------------------------

} // namespace sight::module::ui::series::ut
