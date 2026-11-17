/************************************************************************
 *
 * Copyright (C) 2021-2026 IRCAD France
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

#include <data/set.hpp>
#include <data/string.hpp>

#include <service/base.hpp>
#include <service/op.hpp>

#include <ui/test/dialog/input.hpp>

#include <doctest/doctest.h>

#include <memory>

namespace
{

struct export_set_fixture
{
    export_set_fixture()
    {
        m_export_set = sight::service::add("sight::module::ui::export_set");
        REQUIRE_MESSAGE(m_export_set, "Failed to create service 'sight::module::ui::export_set'");
    }

    ~export_set_fixture()
    {
        if(!m_export_set->stopped())
        {
            CHECK_NOTHROW(m_export_set->stop().get());
        }

        sight::service::remove(m_export_set);
    }

    export_set_fixture(const export_set_fixture&)            = delete;
    export_set_fixture& operator=(const export_set_fixture&) = delete;
    export_set_fixture(export_set_fixture&&)                 = delete;
    export_set_fixture& operator=(export_set_fixture&&)      = delete;

    sight::service::base::sptr m_export_set;
};

} // namespace

TEST_SUITE("sight::module::ui::export_set")
{
    TEST_CASE_FIXTURE(export_set_fixture, "basic")
    {
        auto hello_world = std::make_shared<sight::data::string>("Hello world!");
        auto set         = std::make_shared<sight::data::set>();

        m_export_set->set_inout(hello_world, "data.element");
        m_export_set->set_inout(set, "data.container");
        CHECK(set->empty());

        CHECK_NOTHROW(m_export_set->configure());
        CHECK_NOTHROW(m_export_set->start().get());
        sight::ui::test::dialog::input::push_input("I don't care");
        CHECK_NOTHROW(m_export_set->update().get());
        CHECK_NOTHROW(m_export_set->stop().get());

        CHECK_EQ(std::size_t(1), set->size());
        CHECK((*set)[0] == hello_world);
    }
}
