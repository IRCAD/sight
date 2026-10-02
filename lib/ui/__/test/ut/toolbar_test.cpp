/************************************************************************
 *
 * Copyright (C) 2026 IRCAD France
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

#include <ui/__/builder/toolbar.hpp>

#include <doctest/doctest.h>

namespace sight::ui::ut
{

namespace
{

class test_toolbar : public ui::builder::toolbar
{
public:

    //------------------------------------------------------------------------------

    void create_tool_bar(ui::container::widget::sptr /*_parent*/) override
    {
    }

    //------------------------------------------------------------------------------

    void destroy_tool_bar() override
    {
    }

    //------------------------------------------------------------------------------

    alignment get_alignment() const
    {
        return m_alignment;
    }
};

} // namespace

TEST_SUITE("sight::ui::ut::toolbar")
{
//------------------------------------------------------------------------------

    TEST_CASE("initialize_test")
    {
        test_toolbar toolbar;
        ui::config_t config;

        config.put("<xmlattr>.align", "top");
        toolbar.initialize(config);
        CHECK(toolbar.get_alignment() == ui::builder::toolbar::top);

        config.put("<xmlattr>.align", "bottom");
        toolbar.initialize(config);
        CHECK(toolbar.get_alignment() == ui::builder::toolbar::bottom);

        config.put("<xmlattr>.align", "right");
        toolbar.initialize(config);
        CHECK(toolbar.get_alignment() == ui::builder::toolbar::right);

        config.put("<xmlattr>.align", "left");
        toolbar.initialize(config);
        CHECK(toolbar.get_alignment() == ui::builder::toolbar::left);
    }
} // TEST_SUITE

} // namespace sight::ui::ut
