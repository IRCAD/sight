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

#include "SSelectObjectTest.hpp"

#include <core/com/slot_base.hxx>

#include <data/String.hpp>

#include <service/op.hpp>

CPPUNIT_TEST_SUITE_REGISTRATION(sight::module::data::ut::SSelectObjectTest);

namespace sight::module::data::ut
{

//------------------------------------------------------------------------------

void SSelectObjectTest::setUp()
{
    m_selectObject = service::add("sight::module::data::SSelectObject");
}

//------------------------------------------------------------------------------

void SSelectObjectTest::tearDown()
{
    CPPUNIT_ASSERT_NO_THROW(m_selectObject->stop().get());
    service::remove(m_selectObject);
}

//------------------------------------------------------------------------------

void SSelectObjectTest::basicTest()
{
    auto object1 = std::make_shared<sight::data::String>("Hello");
    auto object2 = std::make_shared<sight::data::String>("world");
    CPPUNIT_ASSERT_NO_THROW(m_selectObject->start().get());
    m_selectObject->slot("add")->run(std::dynamic_pointer_cast<sight::data::Object>(object1));
    CPPUNIT_ASSERT(m_selectObject->output<sight::data::String>("object").lock() == object1);
    m_selectObject->slot("remove")->run();
    CPPUNIT_ASSERT(m_selectObject->output<sight::data::String>("object").lock() == nullptr);
    m_selectObject->slot("add")->run(std::dynamic_pointer_cast<sight::data::Object>(object2));
    CPPUNIT_ASSERT(m_selectObject->output<sight::data::String>("object").lock() == object2);
}

} // namespace sight::module::data::ut
