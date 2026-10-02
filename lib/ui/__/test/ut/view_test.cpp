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

#include <ui/__/detail/registry/view.hpp>

#include <boost/property_tree/xml_parser.hpp>

#include <doctest/doctest.h>

namespace sight::ui::ut
{

namespace
{

/// Forward-declared so test_view can grant it (and only it) access to its protected members.
struct view_test;

class test_view : public ui::detail::registry::view
{
public:

    test_view() = delete;
    explicit test_view(const std::string& _sid) :
        ui::detail::registry::view(_sid)
    {
    }

    ~test_view() override = default;

    friend view_test;
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

TEST_SUITE("sight::ui::ut::view")
{
//------------------------------------------------------------------------------

    TEST_CASE("configuring_test")
    {
        view_test::configuring_test();
    }
} // TEST_SUITE

} // namespace sight::ui::ut
