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

#include "named_widget.hpp"

#include <ui/__/builder/toolbar.hpp>
#include <ui/__/detail/registry/toolbar.hpp>

#include <boost/property_tree/xml_parser.hpp>

#include <doctest/doctest.h>

#include <array>
#include <memory>
#include <sstream>
#include <string>

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

//------------------------------------------------------------------------------

/// Returns the names a toolbar gives the containers of its editors, in the order of its layout.
static std::array<std::string, 3> editor_names(const std::string& _sid, const std::string& _wid)
{
    std::stringstream xml;
    xml << "<registry><editor sid=\"" << _sid << "\"/><editor wid=\"" << _wid << "\"/></registry>";
    boost::property_tree::ptree config;
    boost::property_tree::read_xml(xml, config);

    ui::detail::registry::toolbar toolbar("toolbar_srv");
    toolbar.initialize(config.get_child("registry"));

    // A third container that no editor of the XML takes.
    const std::array containers {
        std::make_shared<named_widget>(),
        std::make_shared<named_widget>(),
        std::make_shared<named_widget>()
    };
    toolbar.manage(std::vector<ui::container::widget::sptr>(containers.begin(), containers.end()));
    toolbar.unmanage();

    return {containers[0]->m_name, containers[1]->m_name, containers[2]->m_name};
}

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

    //------------------------------------------------------------------------------

    TEST_CASE("names_editors_after_their_xml_id")
    {
        // An editor of a toolbar is a service view: it bears its XML uid, as a sub-view of a view does.
        CHECK(
            editor_names("video_selector_srv", "slot_view")
            == std::array<std::string, 3> {"video_selector_srv", "slot_view", ""
            });

        // Ids qualified by a launched configuration instance keep their XML uid only.
        CHECK(
            editor_names("controls-3-video_selector_srv", "controls-3-slot_view")
            == std::array<std::string, 3> {"video_selector_srv", "slot_view", ""
            });
    }
} // TEST_SUITE

} // namespace sight::ui::ut
