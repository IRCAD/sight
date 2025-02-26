/************************************************************************
 *
 * Copyright (C) 2023-2025 IRCAD France
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

#include "export_test.hpp"

#include <data/series.hpp>
#include <data/series_set.hpp>

#include <service/op.hpp>

#include <ui/test/dialog/input.hpp>

#include <utest/wait.hpp>

CPPUNIT_TEST_SUITE_REGISTRATION(sight::module::ui::series::ut::export_test);

namespace sight::module::ui::series::ut
{

//------------------------------------------------------------------------------

void export_test::setUp()
{
    m_export = service::add("sight::module::ui::series::exporter");
    CPPUNIT_ASSERT_MESSAGE("Failed to create service 'sight::module::ui::series::exporter'", m_export);
}

//------------------------------------------------------------------------------

void export_test::tearDown()
{
    if(!m_export->stopped())
    {
        CPPUNIT_ASSERT_NO_THROW(m_export->stop().get());
    }

    service::remove(m_export);
}

//------------------------------------------------------------------------------

void export_test::basic_test()
{
    auto data = std::make_shared<data::series>();
    m_export->set_inout(data, "data");
    auto container = std::make_shared<data::series_set>();
    m_export->set_inout(container, "container");
    CPPUNIT_ASSERT(container->empty());
    CPPUNIT_ASSERT_NO_THROW(m_export->configure());
    CPPUNIT_ASSERT_NO_THROW(m_export->start().get());
    sight::ui::test::dialog::input::push_input("I don't care");
    CPPUNIT_ASSERT_NO_THROW(m_export->update().get());
    CPPUNIT_ASSERT_NO_THROW(m_export->stop().get());
    CPPUNIT_ASSERT_EQUAL(std::size_t(1), container->size());
    CPPUNIT_ASSERT((*container)[0] == data);
}

//------------------------------------------------------------------------------

} // namespace sight::module::ui::series::ut
