/************************************************************************
 *
 * Copyright (C) 2022-2025 IRCAD France
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

#include "export_set_test.hpp"

#include <core/runtime/runtime.hpp>

#include <data/set.hpp>
#include <data/string.hpp>

#include <service/op.hpp>

#include <ui/test/dialog/input.hpp>

#include <utest/wait.hpp>

#include <boost/dll.hpp>

CPPUNIT_TEST_SUITE_REGISTRATION(sight::module::ui::ut::export_set_test);

namespace sight::module::ui::ut
{

//------------------------------------------------------------------------------

void export_set_test::setUp()
{
    m_export_set = service::add("sight::module::ui::export_set");
    CPPUNIT_ASSERT_MESSAGE("Failed to create service 'sight::module::ui::export_set'", m_export_set);
}

//------------------------------------------------------------------------------

void export_set_test::tearDown()
{
    if(!m_export_set->stopped())
    {
        CPPUNIT_ASSERT_NO_THROW(m_export_set->stop().get());
    }

    service::remove(m_export_set);
}

//------------------------------------------------------------------------------

void export_set_test::basic_test()
{
    data::string::sptr hello_world = std::make_shared<data::string>("Hello world!");
    data::set::sptr set            = std::make_shared<data::set>();
    m_export_set->set_inout(hello_world, "data");
    m_export_set->set_inout(set, "container");
    CPPUNIT_ASSERT(set->empty());
    CPPUNIT_ASSERT_NO_THROW(m_export_set->configure());
    CPPUNIT_ASSERT_NO_THROW(m_export_set->start().get());
    sight::ui::test::dialog::input::push_input("I don't care");
    CPPUNIT_ASSERT_NO_THROW(m_export_set->update().get());
    CPPUNIT_ASSERT_NO_THROW(m_export_set->stop().get());
    CPPUNIT_ASSERT_EQUAL(std::size_t(1), set->size());
    CPPUNIT_ASSERT((*set)[0] == hello_world);
}

//------------------------------------------------------------------------------

} // namespace sight::module::ui::ut
