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

#include <data/series.hpp>
#include <data/series_set.hpp>

#include <service/base.hpp>
#include <service/op.hpp>

#include <ui/test/dialog/input.hpp>

#include <doctest/doctest.h>

#include <memory>

namespace
{

struct series_export_fixture
{
    series_export_fixture()
    {
        m_export = sight::service::add("sight::module::ui::series::exporter");
        REQUIRE_MESSAGE(m_export, "Failed to create service 'sight::module::ui::series::exporter'");
    }

    ~series_export_fixture()
    {
        if(!m_export->stopped())
        {
            CHECK_NOTHROW(m_export->stop().get());
        }

        sight::service::remove(m_export);
    }

    series_export_fixture(const series_export_fixture&)            = delete;
    series_export_fixture& operator=(const series_export_fixture&) = delete;
    series_export_fixture(series_export_fixture&&)                 = delete;
    series_export_fixture& operator=(series_export_fixture&&)      = delete;

    sight::service::base::sptr m_export;
};

} // namespace

TEST_SUITE("sight::module::ui::series::exporter")
{
    TEST_CASE_FIXTURE(series_export_fixture, "basic")
    {
        auto data      = std::make_shared<sight::data::series>();
        auto container = std::make_shared<sight::data::series_set>();

        m_export->set_inout(data, "data.element");
        m_export->set_inout(container, "data.container");
        CHECK(container->empty());

        CHECK_NOTHROW(m_export->configure());
        CHECK_NOTHROW(m_export->start().get());
        sight::ui::test::dialog::input::push_input("I don't care");
        CHECK_NOTHROW(m_export->update().get());
        CHECK_NOTHROW(m_export->stop().get());

        CHECK_EQ(std::size_t(1), container->size());
        CHECK((*container)[0] == data);
    }
}
