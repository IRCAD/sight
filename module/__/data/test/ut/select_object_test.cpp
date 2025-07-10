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

#include "select_object_test.hpp"

#include <core/com/slot_base.hxx>

#include <data/string.hpp>

#include <service/op.hpp>

CPPUNIT_TEST_SUITE_REGISTRATION(sight::module::data::ut::select_object_test);

namespace sight::module::data::ut
{

//------------------------------------------------------------------------------

void select_object_test::setUp()
{
    m_select_object = service::add("sight::module::data::select_object");
}

//------------------------------------------------------------------------------

void select_object_test::tearDown()
{
    CPPUNIT_ASSERT_NO_THROW(m_select_object->stop().get());
    service::remove(m_select_object);
}

//------------------------------------------------------------------------------

void select_object_test::basic_test()
{
    auto object1 = std::make_shared<sight::data::string>("Hello");
    auto object2 = std::make_shared<sight::data::string>("world");
    CPPUNIT_ASSERT_NO_THROW(m_select_object->start().get());
    m_select_object->slot("add")->run(std::dynamic_pointer_cast<sight::data::object>(object1));
    CPPUNIT_ASSERT(m_select_object->output<sight::data::string>("object").lock() == object1);
    m_select_object->slot("remove")->run();
    CPPUNIT_ASSERT(m_select_object->output<sight::data::string>("object").lock() == nullptr);
    m_select_object->slot("add")->run(std::dynamic_pointer_cast<sight::data::object>(object2));
    CPPUNIT_ASSERT(m_select_object->output<sight::data::string>("object").lock() == object2);
}

} // namespace sight::module::data::ut
