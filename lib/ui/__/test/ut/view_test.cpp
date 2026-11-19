/************************************************************************
 *
 * Copyright (C) 2016-2026 IRCAD France
 * Copyright (C) 2016-2020 IHU Strasbourg
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

#include <ui/__/detail/registry/view.hpp>

#include <boost/property_tree/xml_parser.hpp>

#include <doctest/doctest.h>

#include <string>
#include <string_view>
#include <utility>

namespace sight::ui::ut
{

namespace
{

class test_view : public ui::detail::registry::view
{
public:

    test_view() = delete;
    explicit test_view(const std::string& _sid) :
        ui::detail::registry::view(_sid)
    {
    }

    ~test_view() override = default;

    // Only this test reaches the protected members.
    friend struct view_test;
};

/// Holds the test body as a static method, so it can be named in the friend declaration above -
/// a TEST_CASE body cannot be, since doctest expands it into an anonymous function.
struct view_test
{
    //------------------------------------------------------------------------------

    static void configuring_test()
    {
        auto view = std::make_shared<test_view>("view");

        std::stringstream xml_config;
        xml_config << ""
                      "<menubar sid=\"myMenu\"/>"
                      "<toolbar sid=\"myToolBar\"/>"
                      "<view sid=\"view1\" />"
                      "<view sid=\"view2\" />"
                      "<slideView sid=\"slideView1\" />"
                      "<view sid=\"view3\" />"
                      "<slideView sid=\"slideView2\" />"
                      "<menubar sid=\"ignored\"/>"
                      "<toolbar sid=\"ignored\"/>"
                      "";
        boost::property_tree::ptree config;
        boost::property_tree::read_xml(xml_config, config);

        view->initialize(config);

        CHECK(view->m_sids.find("view1") != view->m_sids.end());
        CHECK(view->m_sids.find("view2") != view->m_sids.end());
        CHECK(view->m_sids.find("slideView1") != view->m_sids.end());
        CHECK(view->m_sids.find("view3") != view->m_sids.end());
        CHECK(view->m_sids.find("slideView2") != view->m_sids.end());
        CHECK(view->m_sids.find("view4") == view->m_sids.end());
        CHECK(view->m_sids.find("slideView3") == view->m_sids.end());
        CHECK_EQ(std::string("myMenu"), view->m_menu_bar_sid.first);
        CHECK_EQ(std::string("myToolBar"), view->m_tool_bar_sid.first);
    }
};

} // namespace

//------------------------------------------------------------------------------

/// Returns the names a view gives to its sid and wid sub-views.
static std::pair<std::string, std::string> managed_names(
    const std::string& _view,
    std::string_view _sid,
    std::string_view _wid
)
{
    std::stringstream xml_config;
    xml_config << "<registry><view sid=\"" << _sid << "\"/><view wid=\"" << _wid << "\"/></registry>";
    boost::property_tree::ptree config;
    boost::property_tree::read_xml(xml_config, config);

    auto view = std::make_shared<test_view>(_view);
    view->initialize(config.get_child("registry"));

    auto sid_widget = std::make_shared<named_widget>();
    auto wid_widget = std::make_shared<named_widget>();
    view->manage({sid_widget, wid_widget});
    view->unmanage();

    return {sid_widget->m_name, wid_widget->m_name};
}

TEST_SUITE("sight::ui::ut::view")
{
//------------------------------------------------------------------------------

    TEST_CASE("configuring_test")
    {
        view_test::configuring_test();
    }

    TEST_CASE("names_a_service_only_when_its_parent_does_not")
    {
        const auto needs_own_name =
            [](std::string_view _xml, std::string_view _name)
            {
                std::stringstream xml_config;
                xml_config << "<registry>" << _xml << "</registry>";
                boost::property_tree::ptree config;
                boost::property_tree::read_xml(xml_config, config);

                test_view view("view");
                view.initialize(config.get_child("registry"));
                return view.needs_own_name(_name);
            };

        // A <view sid="..."/> of the parent view gives it its name.
        CHECK_FALSE(needs_own_name("", "viewer_srv"));
        CHECK(needs_own_name(R"(<parent wid="scene_view"/>)", "viewer_srv"));

        // The wid already bears that name, also when a launched configuration qualifies it.
        CHECK_FALSE(needs_own_name(R"(<parent wid="scene_view"/>)", "scene_view"));
        CHECK_FALSE(needs_own_name(R"(<parent wid="config-1-scene_view"/>)", "scene_view"));
        CHECK(needs_own_name(R"(<parent wid="config-1-scene_view"/>)", "viewer_srv"));
    }

    //------------------------------------------------------------------------------

    TEST_CASE("names_sub_views_after_their_xml_id")
    {
        const std::pair<std::string, std::string> xml_names {"child_srv", "child_view"};
        CHECK(managed_names("view", "child_srv", "child_view") == xml_names);

        // Ids qualified by a launched configuration instance keep their XML uid only.
        CHECK(
            managed_names(
                "sight::config::viz::scene3d::volumeRendering-12-view",
                "sight::config::viz::scene3d::volumeRendering-12-child_srv",
                "sight::config::viz::scene3d::volumeRendering-12-child_view"
            ) == xml_names
        );
    }
} // TEST_SUITE

} // namespace sight::ui::ut
