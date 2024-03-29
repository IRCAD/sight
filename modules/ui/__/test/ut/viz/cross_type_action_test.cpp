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

#include "cross_type_action_test.hpp"

#include <core/com/slot.hxx>

#include <service/op.hpp>

#include <utest/wait.hpp>

CPPUNIT_TEST_SUITE_REGISTRATION(sight::module::ui::viz::ut::cross_type_action_test);

namespace sight::module::ui::viz::ut
{

//------------------------------------------------------------------------------

void cross_type_action_test::setUp()
{
    m_cross_type_action = service::add("sight::module::ui::viz::cross_type_action");
    CPPUNIT_ASSERT_MESSAGE(
        "Failed to create service 'sight::module::ui::viz::cross_type_action'",
        m_cross_type_action
    );
}

//------------------------------------------------------------------------------

void cross_type_action_test::tearDown()
{
    m_worker->stop();
    if(!m_cross_type_action->stopped())
    {
        CPPUNIT_ASSERT_NO_THROW(m_cross_type_action->stop().get());
    }

    service::remove(m_cross_type_action);
}

//------------------------------------------------------------------------------

void cross_type_action_test::test(const std::string& _cross_type, double _expected_scale)
{
    boost::property_tree::ptree ptree;
    ptree.put("crossType", _cross_type);
    m_cross_type_action->set_config(ptree);
    double scale                  = -1;
    auto cross_type_modified_slot = core::com::new_slot(
        [&scale](double _scale)
        {
            scale = _scale;
        });
    m_worker = core::thread::worker::make();
    cross_type_modified_slot->set_worker(m_worker);
    m_cross_type_action->signal("crossTypeModified")->connect(cross_type_modified_slot);
    CPPUNIT_ASSERT_NO_THROW(m_cross_type_action->configure());
    CPPUNIT_ASSERT_NO_THROW(m_cross_type_action->start().get());

    CPPUNIT_ASSERT_NO_THROW(m_cross_type_action->update().get());
    SIGHT_TEST_WAIT(_expected_scale == scale);
    CPPUNIT_ASSERT_EQUAL(_expected_scale, scale);
}

//------------------------------------------------------------------------------

void cross_type_action_test::full_test()
{
    test("full", 1);
}

//------------------------------------------------------------------------------

void cross_type_action_test::half_test()
{
    test("half", 0.5);
}

//------------------------------------------------------------------------------

void cross_type_action_test::hide_test()
{
    test("hide", 0);
}

//------------------------------------------------------------------------------

} // namespace sight::module::ui::viz::ut
