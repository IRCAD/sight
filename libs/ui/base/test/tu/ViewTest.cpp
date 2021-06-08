/************************************************************************
 *
 * Copyright (C) 2016-2021 IRCAD France
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

#include "ViewTest.hpp"

#include <core/runtime/Convert.hpp>

#include <ui/base/registry/View.hpp>

#include <boost/property_tree/xml_parser.hpp>

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION(sight::ui::base::ut::ViewTest);

namespace sight::ui::base
{

namespace ut
{

class TestView : public ui::base::registry::View
{
public:

    TestView() = delete;
    TestView(const std::string& sid) :
        ui::base::registry::View(sid)
    {
    }

    ~TestView() = default;

    friend ViewTest;
};

//------------------------------------------------------------------------------

void ViewTest::setUp()
{
}

//------------------------------------------------------------------------------

void ViewTest::tearDown()
{
}

//------------------------------------------------------------------------------

void ViewTest::configuringTest()
{
    {
        auto view = std::make_shared<TestView>("view");

        std::stringstream xmlConfig;
        xmlConfig << ""
                     "<registry>"
                     "<menuBar sid=\"myMenu\"/>"
                     "<toolBar sid=\"myToolBar\"/>"
                     "<view sid=\"view1\" start=\"true\"/>"
                     "<view sid=\"view2\" start=\"true\"/>"
                     "<slideView sid=\"slideView1\" start=\"false\"/>"
                     "<view sid=\"view3\" />"
                     "<slideView sid=\"slideView2\" start=\"true\"/>"
                     "<menuBar sid=\"ignored\"/>"
                     "<toolBar sid=\"ignored\"/>"
                     "</registry>"
                     "";
        boost::property_tree::ptree config;
        boost::property_tree::read_xml(xmlConfig, config);
        auto configuration = core::runtime::Convert::fromPropertyTree(config);

        view->initialize(configuration);

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
        CPPUNIT_ASSERT_EQUAL(std::string("myMenu"), view->m_menuBarSid.first);
        CPPUNIT_ASSERT_EQUAL(std::string("myToolBar"), view->m_toolBarSid.first);
    }

    // Test that we throw when the configuration does not start with <registry>
    {
        auto view = std::make_shared<TestView>("view");

        std::stringstream xmlConfig;
        xmlConfig << ""
                     "<not_registry>"
                     "<menuBar sid=\"myMenu\"/>"
                     "<toolBar sid=\"myToolBar\"/>"
                     "<view sid=\"view1\" start=\"true\"/>"
                     "</not_registry>"
                     "";

        boost::property_tree::ptree config;
        boost::property_tree::read_xml(xmlConfig, config);
        auto configuration = core::runtime::Convert::fromPropertyTree(config);

        CPPUNIT_ASSERT_THROW(view->initialize(configuration), core::Exception);

        CPPUNIT_ASSERT(view->m_sids.find("view1") == view->m_sids.end());
        CPPUNIT_ASSERT(view->m_sids.find("menu") == view->m_sids.end());
        CPPUNIT_ASSERT_EQUAL(std::string(""), view->m_menuBarSid.first);
        CPPUNIT_ASSERT_EQUAL(std::string(""), view->m_toolBarSid.first);
    }
}

//------------------------------------------------------------------------------

} //namespace ut

} //namespace sight::ui::base
