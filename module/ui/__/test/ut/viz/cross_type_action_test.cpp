/************************************************************************
 *
 * Copyright (C) 2023-2026 IRCAD France
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

#include <core/com/slot.hpp>
#include <core/thread/worker.hpp>

#include <service/base.hpp>
#include <service/op.hpp>

#include <utest/wait.hpp>

#include <doctest/doctest.h>

#include <string>

namespace
{

struct cross_type_action_fixture
{
    cross_type_action_fixture()
    {
        m_cross_type_action = sight::service::add("sight::module::ui::viz::cross_type_action");
        REQUIRE_MESSAGE(
            m_cross_type_action,
            "Failed to create service 'sight::module::ui::viz::cross_type_action'"
        );
    }

    ~cross_type_action_fixture()
    {
        if(m_worker)
        {
            m_worker->stop();
        }

        if(!m_cross_type_action->stopped())
        {
            CHECK_NOTHROW(m_cross_type_action->stop().get());
        }

        sight::service::remove(m_cross_type_action);
    }

    cross_type_action_fixture(const cross_type_action_fixture&)            = delete;
    cross_type_action_fixture& operator=(const cross_type_action_fixture&) = delete;
    cross_type_action_fixture(cross_type_action_fixture&&)                 = delete;
    cross_type_action_fixture& operator=(cross_type_action_fixture&&)      = delete;

    //------------------------------------------------------------------------------

    void test(const std::string& _cross_type, double _expected_scale)
    {
        boost::property_tree::ptree ptree;
        ptree.put("crossType", _cross_type);
        m_cross_type_action->set_config(ptree);

        double scale                  = -1;
        auto cross_type_modified_slot = sight::core::com::new_slot(
            [&scale](double _scale)
            {
                scale = _scale;
            });

        m_worker = sight::core::thread::worker::make();
        cross_type_modified_slot->set_worker(m_worker);
        m_cross_type_action->signal("crossTypeModified")->connect(cross_type_modified_slot);

        CHECK_NOTHROW(m_cross_type_action->configure());
        CHECK_NOTHROW(m_cross_type_action->start().get());
        CHECK_NOTHROW(m_cross_type_action->update().get());

        SIGHT_TEST_WAIT(_expected_scale == scale);
        CHECK_EQ(_expected_scale, scale);
    }

    sight::service::base::sptr m_cross_type_action;
    sight::core::thread::worker::sptr m_worker;
};

} // namespace

TEST_SUITE("sight::module::ui::viz::cross_type_action")
{
    TEST_CASE_FIXTURE(cross_type_action_fixture, "full")
    {
        test("full", 1);
    }

//------------------------------------------------------------------------------

    TEST_CASE_FIXTURE(cross_type_action_fixture, "half")
    {
        test("half", 0.5);
    }

//------------------------------------------------------------------------------

    TEST_CASE_FIXTURE(cross_type_action_fixture, "hide")
    {
        test("hide", 0);
    }
}
