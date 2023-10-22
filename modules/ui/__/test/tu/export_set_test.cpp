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

#include "export_set_test.hpp"

#include <core/runtime/runtime.hpp>

#include <data/set.hpp>
#include <data/string.hpp>

#include <service/op.hpp>

#include <ui/__/dialog/input_base.hpp>
#include <ui/__/dialog/input_dummy.hpp>
#include <ui/__/macros.hpp>

#include <utest/wait.hpp>

#include <boost/dll.hpp>

CPPUNIT_TEST_SUITE_REGISTRATION(sight::module::ui::ut::export_set_test);

SIGHT_REGISTER_GUI(sight::ui::dialog::input_dummy, sight::ui::dialog::input_base::REGISTRY_KEY);

namespace sight::module::ui::ut
{

//------------------------------------------------------------------------------

void export_set_test::setUp()
{
    m_exportSet = service::add("sight::module::ui::export_set");
    CPPUNIT_ASSERT_MESSAGE("Failed to create service 'sight::module::ui::export_set'", m_exportSet);
}

//------------------------------------------------------------------------------

void export_set_test::tearDown()
{
    if(!m_exportSet->stopped())
    {
        CPPUNIT_ASSERT_NO_THROW(m_exportSet->stop().get());
    }

    service::remove(m_exportSet);
}

//------------------------------------------------------------------------------

void export_set_test::basicTest()
{
    data::string::sptr hello_world = std::make_shared<data::string>("Hello world!");
    data::set::sptr set            = std::make_shared<data::set>();
    m_exportSet->set_inout(hello_world, "data");
    m_exportSet->set_inout(set, "container");
    CPPUNIT_ASSERT(set->empty());
    CPPUNIT_ASSERT_NO_THROW(m_exportSet->configure());
    CPPUNIT_ASSERT_NO_THROW(m_exportSet->start().get());
    sight::ui::dialog::input_dummy::pushInput("I don't care");
    CPPUNIT_ASSERT_NO_THROW(m_exportSet->update().get());
    CPPUNIT_ASSERT_NO_THROW(m_exportSet->stop().get());
    CPPUNIT_ASSERT_EQUAL(std::size_t(1), set->size());
    CPPUNIT_ASSERT((*set)[0] == hello_world);
}

//------------------------------------------------------------------------------

} // namespace sight::module::ui::ut
