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

#include <data/series.hpp>
#include <data/series_set.hpp>
#include <data/vector.hpp>

#include <service/base.hpp>
#include <service/op.hpp>

#include <doctest/doctest.h>

#include <memory>

namespace
{

struct push_selection_fixture
{
    push_selection_fixture()
    {
        m_push_selection = sight::service::add("sight::module::ui::series::push_selection");
        REQUIRE_MESSAGE(
            m_push_selection,
            "Failed to create service 'sight::module::ui::series::push_selection'"
        );
    }

    ~push_selection_fixture()
    {
        if(!m_push_selection->stopped())
        {
            CHECK_NOTHROW(m_push_selection->stop().get());
        }

        sight::service::remove(m_push_selection);
    }

    push_selection_fixture(const push_selection_fixture&)            = delete;
    push_selection_fixture& operator=(const push_selection_fixture&) = delete;
    push_selection_fixture(push_selection_fixture&&)                 = delete;
    push_selection_fixture& operator=(push_selection_fixture&&)      = delete;

    sight::service::base::sptr m_push_selection;
};

} // namespace

TEST_SUITE("sight::module::ui::series::push_selection")
{
    TEST_CASE_FIXTURE(push_selection_fixture, "basic")
    {
        auto selected_series = std::make_shared<sight::data::vector>();
        auto series          = std::make_shared<sight::data::series>();
        selected_series->push_back(series);

        auto series_set = std::make_shared<sight::data::series_set>();

        m_push_selection->set_input(selected_series, "selectedSeries");
        m_push_selection->set_inout(series_set, "seriesSet");
        CHECK(series_set->empty());

        CHECK_NOTHROW(m_push_selection->configure());
        CHECK_NOTHROW(m_push_selection->start().get());
        CHECK_NOTHROW(m_push_selection->update().get());
        CHECK_NOTHROW(m_push_selection->stop().get());

        CHECK_EQ(std::size_t(1), series_set->size());
        CHECK((*series_set)[0] == series);
    }
}
