/************************************************************************
 *
 * Copyright (C) 2016-2023 IRCAD France
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

#include "view_test.hpp"

#include <ui/__/detail/registry/view.hpp>

#include <boost/property_tree/xml_parser.hpp>

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION(sight::ui::ut::view_test);

namespace sight::ui::ut
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

    friend view_test;
};

//------------------------------------------------------------------------------

void view_test::setUp()
{
}

//------------------------------------------------------------------------------

void view_test::tearDown()
{
}

//------------------------------------------------------------------------------

void view_test::configuring_test()
{
    {
        auto view = std::make_shared<test_view>("view");

        std::stringstream xml_config;
        xml_config << ""
                      "<menubar sid=\"myMenu\"/>"
                      "<toolbar sid=\"myToolBar\"/>"
                      "<view sid=\"view1\" start=\"true\"/>"
                      "<view sid=\"view2\" start=\"true\"/>"
                      "<slideView sid=\"slideView1\" start=\"false\"/>"
                      "<view sid=\"view3\" />"
                      "<slideView sid=\"slideView2\" start=\"true\"/>"
                      "<menubar sid=\"ignored\"/>"
                      "<toolbar sid=\"ignored\"/>"
                      "";
        boost::property_tree::ptree config;
        boost::property_tree::read_xml(xml_config, config);

        view->initialize(config);

        CPPUNIT_ASSERT(view->m_sids.find("view1") != view->m_sids.end());
        CPPUNIT_ASSERT_EQUAL(true, view->m_sids.find("view1")->second.second);
        CPPUNIT_ASSERT(view->m_sids.find("view2") != view->m_sids.end());
        CPPUNIT_ASSERT_EQUAL(true, view->m_sids.find("view2")->second.second);
        CPPUNIT_ASSERT(view->m_sids.find("slideView1") != view->m_sids.end());
        CPPUNIT_ASSERT_EQUAL(false, view->m_sids.find("slideView1")->second.second);
        CPPUNIT_ASSERT(view->m_sids.find("view3") != view->m_sids.end());
        CPPUNIT_ASSERT_EQUAL(false, view->m_sids.find("view3")->second.second);
        CPPUNIT_ASSERT(view->m_sids.find("slideView2") != view->m_sids.end());
        CPPUNIT_ASSERT_EQUAL(true, view->m_sids.find("slideView2")->second.second);
        CPPUNIT_ASSERT(view->m_sids.find("view4") == view->m_sids.end());
        CPPUNIT_ASSERT(view->m_sids.find("slideView3") == view->m_sids.end());
        CPPUNIT_ASSERT_EQUAL(std::string("myMenu"), view->m_menu_bar_sid.first);
        CPPUNIT_ASSERT_EQUAL(std::string("myToolBar"), view->m_tool_bar_sid.first);
    }
}

//------------------------------------------------------------------------------

} // namespace sight::ui::ut
